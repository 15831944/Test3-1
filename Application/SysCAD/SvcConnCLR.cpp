#include <stdafx.h> //pkh
#include <afxwin.h>         // MFC core and standard components

//#define _MANAGED  /* exclude compile problems */

#include "gpfuncs.h"
#include "errorlog.h"
#include "flwnode.h"
#include "gpwfuncs.h"
#include "neutralcommon.h"

#include "SvcConn.h"
#include "SvcConnCLR.h"

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

using namespace SysCAD;
using namespace System::Windows::Forms;

using namespace System::Collections::ObjectModel;

#pragma managed

//========================================================================
//
//
//
//========================================================================


CString ToCString(String ^ str)
  {
  // Pin memory so GC can't move it while native function is called
  pin_ptr<const wchar_t> wch = PtrToStringChars(str);

  // Conversion to char* :
  // Can just convert wchar_t* to char* using one of the 
  // conversion functions such as: 
  // WideCharToMultiByte()
  // wcstombs_s()
  // ... etc
  size_t convertedChars = 0;
  size_t  sizeInBytes = ((str->Length + 1) * 2);
  errno_t err = 0;
  char    *ch = (char *)malloc(sizeInBytes);

  err = wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

  if (err != 0)
    LogError("SvcConnCLR", 0, "wcstombs_s  failed!\n");

  return CString(wch) ;
  }

//========================================================================
//
//
//
//========================================================================

ref class CSvcConnectCLRThread
  {
  public:
    //EngineProtocol ^ engineProtocol;

    CSvcConnectCLRThread(CSvcConnect * pConn)
      {
      // There could be sharing problems - when importing and editing simultaneously -  unlikely
      m_pConn = pConn;
      m_Action = gcnew SysCAD::Protocol::Action();
      m_iProcessChangeListsHold=0;
      };
    ~CSvcConnectCLRThread()
      {
      for each (IChannel^ channel in ChannelServices::RegisteredChannels)
        ChannelServices::UnregisterChannel(channel);
      };

    void Startup()
      {

      LogNote("CSvcConnectCLRThread", 0, "Startup");

      BinaryServerFormatterSinkProvider^ serverProv = gcnew BinaryServerFormatterSinkProvider();
      serverProv->TypeFilterLevel = System::Runtime::Serialization::Formatters::TypeFilterLevel::Full;

      BinaryClientFormatterSinkProvider^ clientProv = gcnew BinaryClientFormatterSinkProvider();

      Hashtable^ tcpProps = gcnew Hashtable();
      tcpProps["port"] = "0";
      TcpChannel^ tcpChannel = gcnew TcpChannel(tcpProps, clientProv, serverProv);
      ChannelServices::RegisterChannel(tcpChannel, false);
    }

    bool ConfigSetup()
    {
      config = gcnew Config;
      clientProtocol = gcnew ClientProtocol;
      engineProtocol = gcnew EngineProtocol;

      bool success = false;
      success = config->TestUrl(gcnew Uri("ipc://SysCAD.Service/Global"));

      if (!success) // couldn't connect to service, probably not running -- start one.
        {
        try
          {
          // Shell execute Service with parameters projectPath & configPath.
          Process ^proc = gcnew Process();
          proc->EnableRaisingEvents = false;

          String ^ progFiles = gcnew String(ProgFiles());

          String ^ stencilPath = gcnew String(BaseCfgFiles());
          stencilPath = stencilPath + "Stencils";

          proc->StartInfo->FileName = progFiles + "Service.exe";
          proc->StartInfo->Arguments = "\" " + stencilPath + " \"";
          proc->Start();
          //proc->WaitForExit();
          }
        catch (Exception^)
          {
          // Handle this exception here, basically means the app didn't exist.
          }
        }

      success = false;
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

      return success;  
    };




    bool PrepareForUpgrade(String^ projectName, String^ projectPath)
      {
      config->Syncxxx();

      config->RemoteAddProjectAnyway(projectName, projectPath);

      bool clientSuccess = false;
      bool engineSuccess = false;
      int i=0;
      while ((!((clientSuccess)&&(engineSuccess)))&&(i++ < 20)) //_MUST_ find a better way to handle this! (but only temporary...)
        {
        // Basically need to wait until service is ready.
        Sleep(i*i*i); // Last wait will be 1sec.

        delete clientProtocol;
        delete engineProtocol;
        clientProtocol = gcnew ClientProtocol;
        engineProtocol = gcnew EngineProtocol;

        // Connect to graphic data.

        clientSuccess = clientProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Client/" + projectName));
        engineSuccess = engineProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Engine/" + projectName));
        }
      if (clientSuccess&&engineSuccess)
        {
        clientProtocol->Connect("SysCAD9.1\nConnection: Client");
        engineProtocol->Connect("SysCAD9.1\nConnection: Engine");

        // This will allow the editor to create/delete in addition to modify after the project loads.
        __int64 requestId;
        engineProtocol->ChangeState(requestId, SysCAD::Protocol::EngineBaseProtocol::RunState::Edit);

        engineProtocol->PortInfoRequested += gcnew EngineProtocol::PortInfoRequestedHandler(this, &CSvcConnectCLRThread::PortInfoRequested);

        /*
        This is where import goes
        */

        clientProtocol->Changed += gcnew ClientProtocol::ChangedHandler(this, &CSvcConnectCLRThread::Changed);

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

        //for each (GraphicItem ^ item in clientProtocol->graphicItems->Values)
        //{
        // 'Go To Definition' on GraphicItem doesn't go to the source but does show the
        // ObjectBrowser with all the available members.
        // e.g.
        // item->Angle
        // item->X

        //CNM Removed must work out when to do this
        //m_pConn->OnCreateItem(-1, -1, ToCString(item->Guid.ToString()), ToCString(item->Tag), ToCString(item->Path), 
        //  ToCString(item->Model->ToString()), ToCString(item->Shape->ToString()), 
        //  CRectangleF(item->BoundingRect.Left, item->BoundingRect.Right, item->BoundingRect.Bottom, item->BoundingRect.Top), 
        //  item->Angle, RGB(item->FillColor.R, item->FillColor.G, item->FillColor.B), 
        //  item->MirrorX, item->MirrorY);
        //int yyy=0;
        //Chris
        //}

        //for each (GraphicLink ^ link in clientProtocol->graphicLinks->Values)
        //  {
        //  }

        //for each (GraphicThing ^ thing in clientProtocol->graphicThings->Values)
        //  {
        //  }



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

      if (clientProtocol == nullptr)
        return false;

      if ((clientProtocol->graphic->Areas == nullptr) || (clientProtocol->graphic->Nodes == nullptr) || (clientProtocol->graphic->Links == nullptr))
        return false;

      //return (!((clientProtocol->graphic->Groups->Count == 0)&&
      //  (clientProtocol->graphic->Nodes->Count == 0)&&
      //  (clientProtocol->graphic->Links->Count == 0)&&
      //  (clientProtocol->graphic->Things->Count == 0)));

      
      return true;
      };

    void Attach2Scd10()
      {
      //config = gcnew SysCAD::Protocol::Config();

      AttachProjectForm^ attachProjectForm = gcnew AttachProjectForm();
      if (attachProjectForm->ShowDialog() == DialogResult::OK)
        {
        attachProjectForm->ClientProtocol->Connect("SysCAD9.1\nConnection: Client");
        attachProjectForm->Config->Syncxxx();

        config = attachProjectForm->Config;
        clientProtocol = attachProjectForm->ClientProtocol;


        // Connect to graphic data.

        engineProtocol = gcnew EngineProtocol();
        engineProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Engine/" + clientProtocol->Name));
        engineProtocol->Connect("SysCAD9.1\nConnection: Engine");

        // This will allow the editor to create/delete in addition to modify after the project loads.
        __int64 requestId;
        engineProtocol->ChangeState(requestId, SysCAD::Protocol::EngineBaseProtocol::RunState::Edit);

        engineProtocol->PortInfoRequested += gcnew EngineProtocol::PortInfoRequestedHandler(this, &CSvcConnectCLRThread::PortInfoRequested);

        //clientProtocol->Changed += gcnew ClientProtocol::ChangedHandler(this, &CSvcConnectCLRThread::Changed);

        // now load 9
        m_pConn->DoOpenProject(ToCString(clientProtocol->Name), ToCString(clientProtocol->Path));

        // turf 9 grfs
        m_pConn->Remove9Graphics();
        
        // load grfs from 10  
        LoadGraphicsFromClient(clientProtocol->Path, clientProtocol->graphic);

        // check model list consistent
        CheckModels(clientProtocol->Path, clientProtocol->model);
    
        }
      };

    // ====================================================================

    void LoadGraphicsFromClient(String ^ path, SysCAD::Protocol::Graphic ^ Graphic)
      {
      m_pConn->dbgPrintLn("LoadGraphicsFromClient ---------------------------");

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicArea ^> KGrp in clientProtocol->graphic->Areas)
        {
        SysCAD::Protocol::GraphicArea ^ Grp = KGrp.Value;
        m_pConn->dbgPrintLn("Grf Grp: %s", ToCString(Grp->Tag->ToString()));
        m_pConn->GCBCreateGroup(ToCString(Grp->Guid.ToString()), ToCString(path->ToString()), ToCString(Grp->Tag->ToString()), CRectangleF(Grp->BoundingRect->Left, Grp->BoundingRect->Top, Grp->BoundingRect->Width, Grp->BoundingRect->Height));
        }

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicNode ^> KNd in clientProtocol->graphic->Nodes)
        {
        SysCAD::Protocol::GraphicNode ^ GNd = KNd.Value;
        m_pConn->dbgPrintLn("Grf Nd : %s", ToCString(GNd->Tag->ToString()));

        ModelNode ^MNd;
        if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
          {
          __int64 eventId=-1;
          __int64 requestId=-1;
          m_pConn->OnCreateNodeG(eventId, requestId, 
            CSvcHeaderBlk(ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), ToCString(MNd->NodeClass)), 
            CSvcNdGBlk(ToCString(GNd->Stencil), CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), float(GNd->Angle),
            RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), GNd->MirrorX, GNd->MirrorY), 
            CSvcTagBlk(CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height), float(GNd->TagAngle), GNd->TagVisible));
          }
        }

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicLink ^> KVPair in clientProtocol->graphic->Links)
        {
        SysCAD::Protocol::GraphicLink ^ GLnk = KVPair.Value;
        m_pConn->dbgPrintLn("Grf Lnk: %s", ToCString(GLnk->Tag->ToString()));

        CPointFList Pts;
        for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
          Pts.AddTail(CPointF(Pt->X, Pt->Y));
        __int64 eventId=-1;
        __int64 requestId=-1;
        m_pConn->OnCreateLinkG(eventId, requestId, 
          CSvcHeaderBlk(ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", ""),
          CSvcGuidPair(ToCString(GLnk->Origin.ToString()), ToCString(GLnk->Destination.ToString())),
          Pts, 
          CSvcTagBlk(CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height),
          float(GLnk->TagAngle), GLnk->TagVisible));

        }
      }

    // ====================================================================

    void CheckModels(String ^ path, SysCAD::Protocol::Model ^ Model)
      {
      m_pConn->dbgPrintLn("CheckModels -------------------------------------- STILL TO DO");

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::ModelNode ^> KNd in clientProtocol->model->Nodes)
        {
        SysCAD::Protocol::ModelNode ^ Nd = KNd.Value;
        m_pConn->dbgPrintLn("Mdl Nd : %s", ToCString(Nd->Tag->ToString()));
        }

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::ModelLink ^> KVPair in clientProtocol->model->Links)
        {
        SysCAD::Protocol::ModelLink ^ Lnk = KVPair.Value;
        m_pConn->dbgPrintLn("Mdl Lnk: %s", ToCString(Lnk->Tag->ToString()));
        }
      }

    // ====================================================================
    //
    // ====================================================================

    void AddCreateGroup(__int64 & requestId, LPCSTR GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
      {
      GraphicArea ^ Grp = gcnew GraphicArea(Guid(gcnew String(GrpGuid)), gcnew String(Tag), 
        gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height()),
        gcnew String(Path));


      m_Action->Create->Add(Grp);

      ProcessChangeLists(requestId);
      }

    // ====================================================================
    //
    // ====================================================================

    void AddCreateNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
      double Angle, CSvcTagBlk & TagBlk, COLORREF FillColor, 
      bool MirrorX, bool MirrorY)
      {
      if (stricmp(Tag, "A22-SC-001")==0)
        {
        int xxx=0;
        }
      SysCAD::Protocol::Rectangle ^ BndRect = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      SysCAD::Protocol::Rectangle ^ TagArea = gcnew SysCAD::Protocol::Rectangle(TagBlk.m_Area.Left(), TagBlk.m_Area.Bottom(), TagBlk.m_Area.Width(), TagBlk.m_Area.Height());  
      //PKH Comment- Is this correct
      GraphicStencil ^Gs; 

      // Check for Existance - then : Try Trimming
      bool found = false;
      String ^ gcSymbol = gcnew String(Symbol);

      while (gcSymbol->Length>1 && !found)
        {
        for each (GraphicStencil ^ graphicStencil in config->GraphicStencils->Values)
          {
          for each (String ^ tag in graphicStencil->Tags)
            {  
            if (tag->ToLower()->Replace(' ', '_') == gcSymbol->ToLower()->Replace(' ', '_'))
              {
              gcSymbol = tag;
              Gs = graphicStencil;
              found = true;
              }
            }
          }
          if (!found)
            gcSymbol = gcSymbol->Remove(gcSymbol->Length-1);
        }

      if (!found) gcSymbol = gcnew String(Symbol);

      // Try ClassId
      if (Gs == nullptr)
        {
        gcSymbol = (gcnew String(ClassId))->ToLower()->Replace(' ', '_');
        if (gcSymbol->Length>1 && !config->GraphicStencils->TryGetValue(gcSymbol, Gs))
           Gs = nullptr;
        }
      
      if (Gs != nullptr)
        {
        if (BndRect->Width==0.0)
          {
          BndRect->Inflate(Gs->DefaultSize->Width/2.0, Gs->DefaultSize->Height/2.0);
          }
        if (TagArea->Width==0.0)
          {
          TagArea = Gs->TagArea->Clone();
          TagArea->X += BndRect->X;
          TagArea->Y += BndRect->Y;
          }
        }
      else
        {
        // Do Anything ???;
        }
      
      bool HasModel = ModelGuid!=NULL && strlen(ModelGuid)>0;
      bool HasGraphic = GraphicGuid!=NULL && strlen(GraphicGuid)>0;
      Guid MdlGuid = HasModel ? Guid(gcnew String(ModelGuid)) : Guid::Empty;
      Guid GrfGuid = HasGraphic ? Guid(gcnew String(GraphicGuid)) : Guid::NewGuid();

      GraphicNode ^ GNd = gcnew GraphicNode(GrfGuid, gcnew String(Tag), gcnew String(Path), MdlGuid, 
        gcSymbol, BndRect,
        Angle, TagArea, TagBlk.m_Angle, gcnew SysCAD::Protocol::Font(TagBlk.m_Area.Height()), TagBlk.m_Visible, Color::Empty, 
        Drawing2D::FillMode::Alternate, MirrorX, MirrorY);
      m_Action->Create->Add(GNd);

      if (HasModel)
        {
        ModelNode ^ MNd = gcnew ModelNode(MdlGuid, gcnew String(Tag), gcnew String(ClassId));
        m_Action->Create->Add(MNd);
        }
      
      ProcessChangeLists(requestId);
      }


    // ====================================================================
    //
    // ====================================================================

    void AddDeleteNode(__int64 & requestId, LPCSTR GraphicGuid)
      {
      m_Action->Delete->Add(Guid(gcnew String(GraphicGuid)));

      ProcessChangeLists(requestId);
      };

    // ====================================================================
    //
    // ====================================================================

    void AddModifyNodePosition(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta)
      {
      Guid graphicGuid(gcnew String(GraphicGuid));
      GraphicNode ^ GNd;
      GraphicLink ^ GLk;
      if (clientProtocol->graphic->Nodes->TryGetValue(graphicGuid, GNd))
        {

        GraphicNode ^ newGNd = GNd->Clone();
        newGNd->BoundingRect->X += (float)Delta.X;
        newGNd->BoundingRect->Y += (float)Delta.Y;
        newGNd->TagArea->X += (float)Delta.X;
        newGNd->TagArea->Y += (float)Delta.Y;

        m_Action->Modify->Add(newGNd);

        ProcessChangeLists(requestId);
        }
      else if (clientProtocol->graphic->Links->TryGetValue(graphicGuid, GLk))
        {

        GraphicLink ^ newGLk = GLk->Clone();
        for each (SysCAD::Protocol::Point^ Pt in newGLk->ControlPoints)
          {
          Pt->X = Pt->X + (float)Delta.X;
          Pt->Y = Pt->Y + (float)Delta.Y;
          }
        newGLk->TagArea->X += (float)Delta.X;
        newGLk->TagArea->Y += (float)Delta.Y;

        m_Action->Modify->Add(newGLk);
        ProcessChangeLists(requestId);
        }
      };

    void AddModifyTagG(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta, CSvcTagBlk & TagBlk)
      {
      Guid graphicGuid(gcnew String(GraphicGuid));

      GraphicNode ^ GNd;
      if (clientProtocol->graphic->Nodes->TryGetValue(graphicGuid, GNd))
        {
        GraphicNode ^ newGNd = GNd->Clone();

        float OldW = (float)newGNd->TagArea->Width;
        float NewW = (float)(OldW * TagBlk.m_Area.Height()/newGNd->TagArea->Height);
        newGNd->TagArea->X += (float)Delta.X-0.5*(NewW-OldW);
        newGNd->TagArea->Y += (float)Delta.Y;
        newGNd->TagArea->Width = NewW;
        newGNd->TagArea->Height = TagBlk.m_Area.Height();
        newGNd->TagAngle = TagBlk.m_Angle;
        newGNd->TagVisible = TagBlk.m_Visible;

        m_Action->Modify->Add(newGNd);
        ProcessChangeLists(requestId);
        return;
        }

      GraphicLink ^ LNd;
      if (clientProtocol->graphic->Links->TryGetValue(graphicGuid, LNd))
        {
        GraphicLink ^ newLNd = LNd->Clone();

        float OldW = (float)newLNd->TagArea->Width;
        float NewW = (float)(OldW * TagBlk.m_Area.Height()/newLNd->TagArea->Height);
        newLNd->TagArea->X += (float)Delta.X-0.5*(NewW-OldW);
        newLNd->TagArea->Y += (float)Delta.Y;
        newLNd->TagArea->Width = NewW;
        newLNd->TagArea->Height = TagBlk.m_Area.Height();
        newLNd->TagAngle = TagBlk.m_Angle;
        newLNd->TagVisible = TagBlk.m_Visible;

        m_Action->Modify->Add(newLNd);
        ProcessChangeLists(requestId);
        return;
        }
      };

    // ====================================================================
    //
    // ====================================================================

    void AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, 
      LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, 
      LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
      LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
      LPCSTR OriginPort, LPCSTR DestinationPort, 
      CPointFList & ControlPoints, CSvcTagBlk & TagBlk)
      {
      List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
      POSITION Pos=ControlPoints.GetHeadPosition();
      while (Pos)
        {
        CPointF & Pt=ControlPoints.GetNext(Pos);
        Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
        }

      try
        {

        GraphicLink ^ GLnk = gcnew GraphicLink(Guid(gcnew String(GraphicGuid)), Guid(gcnew String(ModelGuid)), gcnew String(Tag), 
          Guid(gcnew String(OriginGrfGuid)), 0, Guid(gcnew String(DestinationGrfGuid)), 0,  
          Pts, gcnew SysCAD::Protocol::Rectangle(TagBlk.m_Area.Left(), TagBlk.m_Area.Bottom(), TagBlk.m_Area.Width(), TagBlk.m_Area.Height()), TagBlk.m_Angle, gcnew SysCAD::Protocol::Font(TagBlk.m_Area.Height()), TagBlk.m_Visible);

        ModelLink ^ MLnk = gcnew ModelLink(Guid(gcnew String(ModelGuid)), gcnew String(Tag), gcnew String(ClassId),
          Guid(gcnew String(OriginMdlGuid)), Guid(gcnew String(DestinationMdlGuid)), gcnew String(OriginPort), gcnew String(DestinationPort));

        m_Action->Create->Add(MLnk);
        m_Action->Create->Add(GLnk);
        ProcessChangeLists(requestId);
        }
      catch (Exception^)
        {
        }
      };

    // ====================================================================
    //
    // ====================================================================

    void AddDeleteLink(__int64 & requestId, LPCSTR GraphicGuid)
      {
      try
        {

        Guid ^ guid = gcnew Guid(gcnew String(GraphicGuid));
        SysCAD::Protocol::GraphicLink ^GLnk;

        if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
          {
          m_Action->Delete->Add(GLnk->ModelGuid);

          //m_pConn->dbgPrintLn("Init  Delete Lnk Mdl  : %s", ToCString(GLnk->ModelGuid.ToString()));

          for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicLink ^> KVPair in clientProtocol->graphic->Links)
            {
            SysCAD::Protocol::GraphicLink ^ GLnk1 = KVPair.Value;
            if (GLnk->ModelGuid == GLnk1->ModelGuid)
              {
              m_Action->Delete->Add(GLnk1->Guid);
              //m_pConn->dbgPrintLn("Check Delete Lnk Grf  : %s", ToCString(GLnk1->Guid.ToString()));
              }
            } 
          }
        ProcessChangeLists(requestId);
        }
      catch (Exception^)
        {
        }
      };

    // ====================================================================
    //
    // ====================================================================


    void AddModifyLinkPts(__int64 & requestId, LPCSTR GraphicGuid, CPointFList & ControlPoints)
      {
      try
        {
        Guid graphicGuid(gcnew String(GraphicGuid));
        GraphicLink ^ GLk;
        if (clientProtocol->graphic->Links->TryGetValue(graphicGuid, GLk))
          {
          GraphicLink ^ newGLk = GLk->Clone();

          List<SysCAD::Protocol::Point^> ^ Pts = newGLk->ControlPoints;
          Pts->Clear();
          POSITION Pos=ControlPoints.GetHeadPosition();
          while (Pos)
            {
            CPointF & Pt=ControlPoints.GetNext(Pos);
            Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
            }

          m_Action->Modify->Add(newGLk);
          ProcessChangeLists(requestId);
          }
        }
      catch (Exception^)
        {
        }

      //void AddModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, CSvcGuidPair & Guids, LPCSTR OriginPort, LPCSTR DestinationPort, 
      //  CPointFList & ControlPoints, CSvcTagBlk & TagBlk)
      //  {
      //  List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
      //  POSITION Pos=ControlPoints.GetHeadPosition();
      //  while (Pos)
      //    {
      //    CPointF & Pt=ControlPoints.GetNext(Pos);
      //    Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
      //    }
      //  SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(TagBlk.m_Area.Left(), TagBlk.m_Area.Bottom(), TagBlk.m_Area.Width(), TagBlk.m_Area.Height());
      };

    // ====================================================================
    //
    // ====================================================================

    void ThingCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect, String^ xaml, double angle, bool mirrorX, bool mirrorY)
      {
      }

    void ThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
      {
      }

    void ThingModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect, String^ xaml, double angle, bool mirrorX, bool mirrorY)
      {
      }

    void ThingPathModified(Int64 eventId, Int64 requestId, Guid guid, String^ path)
      {
      }

    bool ProcessChangeLists(Int64 requestId)
      {
      if (m_iProcessChangeListsHold<=0)
        {
        bool Ret=clientProtocol->Change(requestId, m_Action);
        m_Action->Clear();
        return Ret;
        }
      return true;
      }

    void ProcessChangeListsHold(bool On)
      {
      if (On)
        m_iProcessChangeListsHold++;
      else
        m_iProcessChangeListsHold--;
      };

    void PortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String^ tag)
      {
      PortInfo^ portInfo = gcnew PortInfo(PortStatusEnum::Available);
      engineProtocol->RequestPortInfo(requestId, guid, tag, portInfo);
      }

    void Changed(Int64 eventId, Int64 requestId, SysCAD::Protocol::Actioned ^actioned)
      {
      if (actioned->Created != nullptr)
        ProcessCreatedList(eventId, requestId, actioned->Created);

      if (actioned->Modified != nullptr)
        ProcessModifiedList(eventId, requestId, actioned->Modified);

      if (actioned->Deleted != nullptr)
        ProcessDeletedList(eventId, requestId, actioned->Deleted);
      }

    void ProcessCreatedList(Int64 eventId, Int64 requestId, Collection<Guid> ^ created)
      {
      for each (Guid ^ guid in created )
        {
        GraphicArea ^GGrp;
        ModelNode ^ MNd;
        GraphicNode ^GNd;
        ModelLink ^ MLnk;
        GraphicLink ^GLnk;

        if (clientProtocol->graphic->Areas->TryGetValue(*guid, GGrp))
          {
          m_pConn->OnCreateGroup(eventId, requestId, ToCString(GGrp->Guid.ToString()), ToCString(GGrp->Tag), ToCString(GGrp->Path), CRectangleF(GGrp->BoundingRect->Left, GGrp->BoundingRect->Top, GGrp->BoundingRect->Width, GGrp->BoundingRect->Height)); 
          }

        else if (clientProtocol->model->Nodes->TryGetValue(*guid, MNd))
          {
          m_pConn->OnCreateNodeM(eventId, requestId, CSvcHeaderBlk(ToCString(MNd->Guid.ToString()), ToCString(MNd->Tag), "", ToCString(MNd->NodeClass)));
          }

        else if (clientProtocol->graphic->Nodes->TryGetValue(*guid, GNd))
          {
          ModelNode ^MNd;
          if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
            {
            m_pConn->OnCreateNodeG(eventId, requestId, 
              CSvcHeaderBlk(ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), ToCString(MNd->NodeClass)),
              CSvcNdGBlk(ToCString(GNd->Stencil), CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), float(GNd->Angle),
              RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), GNd->MirrorX, GNd->MirrorY), 
              CSvcTagBlk(CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height), float(GNd->TagAngle), GNd->TagVisible));
            }
          else
            {
            m_pConn->OnCreateNodeG(eventId, requestId, 
              CSvcHeaderBlk(ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), ""),
              CSvcNdGBlk(ToCString(GNd->Stencil), CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), float(GNd->Angle),
              RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), GNd->MirrorX, GNd->MirrorY), 
              CSvcTagBlk(CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height), float(GNd->TagAngle), GNd->TagVisible));
            }
          }

        else if (clientProtocol->model->Links->TryGetValue(*guid, MLnk))
          {
          m_pConn->OnCreateLinkM(eventId, requestId, 
            CSvcHeaderBlk(ToCString(MLnk->Guid.ToString()), ToCString(MLnk->Tag), "", ToCString(MLnk->LinkClass)), 
            CSvcGuidPair(ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString())), ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort));
          }

        else if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
          {

          CPointFList Pts;
          for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
            {
            Pts.AddTail(CPointF(Pt->X, Pt->Y));
            }
          m_pConn->OnCreateLinkG(eventId, requestId, 
            CSvcHeaderBlk(ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", ""),
            CSvcGuidPair(ToCString(GLnk->Origin.ToString()), ToCString(GLnk->Destination.ToString())),
            Pts, 
            CSvcTagBlk(CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height), float(GLnk->TagAngle), GLnk->TagVisible));
          }
        }
      }

    void ProcessModifiedList(Int64 eventId, Int64 requestId, Collection<Guid> ^ modified)
      {
      for each (Guid ^ guid in modified)
        {
        ModelNode ^ MNd;
        if (engineProtocol->model->Nodes->TryGetValue(*guid, MNd))
          {
          m_pConn->OnModifyNodeM(eventId, requestId, 
            CSvcHeaderBlk(ToCString(MNd->Guid.ToString()), ToCString(MNd->Tag), "", ToCString(MNd->NodeClass)));
          }

        GraphicNode ^GNd;
        if (clientProtocol->graphic->Nodes->TryGetValue(*guid, GNd))
          {
          ModelNode ^MNd;
          if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
            {
            m_pConn->OnModifyNodeG(eventId, requestId, 
              CSvcHeaderBlk(ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), ToCString(MNd->NodeClass)),
              CSvcNdGBlk(ToCString(GNd->Stencil), CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), float(GNd->Angle),
              RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), GNd->MirrorX, GNd->MirrorY), 
              CSvcTagBlk(CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height), float(GNd->TagAngle), GNd->TagVisible));
            }
          }

        ModelLink ^ MLnk;
        if (engineProtocol->model->Links->TryGetValue(*guid, MLnk))
          {
          m_pConn->OnModifyLinkM(eventId, requestId, 
            CSvcHeaderBlk(ToCString(MLnk->Guid.ToString()), ToCString(MLnk->Tag), "", ToCString(MLnk->LinkClass)), 
            CSvcLnkMBlk(ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString()), "", "", ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort)));
          }

        GraphicLink ^GLnk;
        if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
          {

          CPointFList Pts;
          for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
            {
            Pts.AddTail(CPointF(Pt->X, Pt->Y));
            }
          m_pConn->OnModifyLinkG(eventId, requestId, 
            CSvcHeaderBlk(ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", ""),
            CSvcGuidPair(ToCString(GLnk->Origin.ToString()), ToCString(GLnk->Destination.ToString())),
            Pts, 
            CSvcTagBlk(CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height), float(GLnk->TagAngle), GLnk->TagVisible));
          }
        }
      }

    void ProcessDeletedList(Int64 eventId, Int64 requestId, Collection<Guid> ^ deleted)
      {
      for each (Guid ^ guid in deleted )
        {
        m_pConn->OnDeleteNodeM(eventId, requestId, CSvcHeaderBlk(ToCString(guid->ToString())));

        m_pConn->OnDeleteNodeG(eventId, requestId, CSvcHeaderBlk(ToCString(guid->ToString())));

        m_pConn->OnDeleteLinkM(eventId, requestId, CSvcHeaderBlk(ToCString(guid->ToString())));

        m_pConn->OnDeleteLinkG(eventId, requestId, CSvcHeaderBlk(ToCString(guid->ToString())));
        }
      }

    void Shutdown()
      {
      delete config;//

      LogNote("CSvcConnectCLRThread", 0, "Shutdown");
      };

    void Load()
      {
      Int64 requestId;
      clientProtocol->Load(requestId);
      };

    void Save()
      {
      Int64 requestId;
      clientProtocol->Save(requestId);
      };

    void LogMessage(DWORD Type, LPCSTR Msg)
      {
      Int64 requestId;
      SysCAD::Log::MessageType T;

      //const DWORD LogFlag_CondClear     = 0x00000001;
      //const DWORD LogFlag_Cond          = 0x00000002;
      //const DWORD LogFlag_RngClear      = 0x00000004;
      //const DWORD LogFlag_Rng           = 0x00000008;
      //const DWORD LogFlag_Fatal         = 0x00000010;
      //const DWORD LogFlag_Stop          = 0x00000020;
      //const DWORD LogFlag_Error         = 0x00000040;
      //const DWORD LogFlag_Warning       = 0x00000080;
      //const DWORD LogFlag_Note          = 0x00000100;
      //const DWORD LogFlag_Cmd           = 0x00000200;
      //const DWORD LogFlag_Separator     = 0x00000400;

      //const DWORD LogFlag_AutoClear     = 0x00001000;
      //const DWORD LogFlag_ClearErrList  = 0x00002000;
      //const DWORD LogFlag_ClearCondList = 0x00004000;
      //const DWORD LogFlag_ClearRngList  = 0x00008000;
      //const DWORD LogFlag_FromPGM       = 0x00010000;
      //const DWORD LogFlag_FromCOM       = 0x00020000;
      //const DWORD LogFlag_FromBlackBox  = 0x00040000;

      if (Type & LogFlag_Note)          T=SysCAD::Log::MessageType::Note;
      else if (Type & LogFlag_Warning)  T=SysCAD::Log::MessageType::Warning;
      else /*if(Type & LogFlag_Error)*/ T=SysCAD::Log::MessageType::Error;

      if (clientProtocol != nullptr)
        clientProtocol->LogMessage(requestId, gcnew String(Msg), T);
      };

  protected:
    SysCAD::Protocol::Config ^ config;
    SysCAD::Protocol::ClientProtocol ^ clientProtocol;
    SysCAD::Protocol::EngineProtocol ^ engineProtocol;

    CSvcConnect   * m_pConn;

    int                       m_iProcessChangeListsHold;
    SysCAD::Protocol::Action ^m_Action;


  };


ref class CSvcConnectCLRThreadGlbl
  {
  public:
    static CSvcConnectCLRThread ^ gs_SrvrThread;// = gcnew CSvcConnectCLRThread;
  };

//========================================================================
//
//
//
//========================================================================

CSvcConnectCLR::CSvcConnectCLR(void)
  {
  }

CSvcConnectCLR::~CSvcConnectCLR(void)
  {
  }

void CSvcConnectCLR::Startup(CSvcConnect * pConn)
  {
  LogNote("CSvcConnectCLR", 0, "Startup");

  m_pConn =  pConn;
  //m_pSrvr->Initialise();

  CSvcConnectCLRThreadGlbl::gs_SrvrThread = gcnew CSvcConnectCLRThread(m_pConn);//.Startup("");

  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Startup();   
  };

bool CSvcConnectCLR::ConfigSetup(CSvcConnect * pConn)
  {
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->ConfigSetup();   

  }

bool CSvcConnectCLR::PrepareForUpgrade(LPCSTR projectName, LPCSTR projectPath)
  {
  String^ projectNameString = gcnew String(projectName);
  String^ projectPathString = gcnew String(projectPath);
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->PrepareForUpgrade(projectNameString, projectPathString);   
  };

void CSvcConnectCLR::Attach2Scd10()
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Attach2Scd10();
  };

void CSvcConnectCLR::Shutdown()
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Shutdown();
  delete CSvcConnectCLRThreadGlbl::gs_SrvrThread;

  //m_pSrvr->Terminate();
  m_pConn = NULL;

  LogNote("CSvcConnectCLR", 0, "Shutdown");
  };

//========================================================================

void CSvcConnectCLR::AddCreateGroup(__int64 & requestId, LPCSTR GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateGroup(requestId, GrpGuid, Tag, Path, boundingRect);
  };

//========================================================================

void CSvcConnectCLR::AddCreateNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path,  
                                   LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                                   double Angle, CSvcTagBlk & TagBlk, COLORREF FillColor, 
                                   bool MirrorX, bool MirrorY)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateNode(requestId, ModelGuid, GraphicGuid, Tag, Path, 
    ClassId, Symbol, boundingRect, Angle, TagBlk, FillColor, MirrorX, MirrorY);
  };

void CSvcConnectCLR::AddDeleteNode(__int64 & requestId, LPCSTR GraphicGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddDeleteNode(requestId, GraphicGuid);
  };

void CSvcConnectCLR::AddModifyNodePosition(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddModifyNodePosition(requestId, GraphicGuid, Delta);
  }

void CSvcConnectCLR::AddModifyTagG(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta, CSvcTagBlk & TagBlk)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddModifyTagG(requestId, GraphicGuid, Delta, TagBlk);
  }

//========================================================================

void CSvcConnectCLR::AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                                   LPCSTR ClassId, 
                                   LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
                                   LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
                                   LPCSTR OriginPort, LPCSTR DestinationPort, 
                                   CPointFList & ControlPoints, CSvcTagBlk & TagBlk)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateLink(requestId, ModelGuid, GraphicGuid, Tag, Path, 
    ClassId, 
    OriginMdlGuid, DestinationMdlGuid, 
    OriginGrfGuid, DestinationGrfGuid, 
    OriginPort, DestinationPort, 
    ControlPoints, TagBlk);
  };

void CSvcConnectCLR::AddDeleteLink(__int64 & requestId, LPCSTR GraphicGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddDeleteLink(requestId, GraphicGuid);
  };

void CSvcConnectCLR::AddModifyLinkPts(__int64 & requestId, LPCSTR GraphicGuid, CPointFList & ControlPoints)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddModifyLinkPts(requestId, GraphicGuid, ControlPoints);
  }

//void CSvcConnectCLR::AddModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, 
//                                  LPCSTR ClassId, 
//                                  CSvcGuidPair & Guids, 
//                                  LPCSTR OriginPort, LPCSTR DestinationPort, 
//                                  CPointFList & ControlPoints, CSvcTagBlk & TagBlk)
//  {
//  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddModifyLink(requestId, LinkGuid, Tag, Path, 
//    ClassId, 
//    Guids, OriginPort, DestinationPort, 
//    ControlPoints, TagBlk);
//  };


//========================================================================

bool CSvcConnectCLR::ProcessChangeLists(__int64 & requestId)
  {
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->ProcessChangeLists(requestId);
  }

void CSvcConnectCLR::ProcessChangeListsHold(bool On)
  {
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->ProcessChangeListsHold(On);
  }

//========================================================================

void CSvcConnectCLR::Load()
  {
  LogNote("CSvcConnectCLR", 0, "Load");

  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Load();
  };

void CSvcConnectCLR::Save()
  {
  LogNote("CSvcConnectCLR", 0, "Save");

  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Save();
  };

//========================================================================

void CSvcConnectCLR::LogMessage(DWORD Type, LPCSTR Msg)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->LogMessage(Type, Msg);
  };

//========================================================================

#endif

//========================================================================
