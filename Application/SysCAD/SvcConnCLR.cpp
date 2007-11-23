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
      m_Create = gcnew List<Item^>; 
      m_Modify = gcnew List<Item^>;
      m_Delete = gcnew List<Guid>;  
      };
    ~CSvcConnectCLRThread()
      {
      };

    bool Startup(String^ configPath)
      {
        
      LogNote("CSvcConnectCLRThread", 0, "Startup");

      BinaryServerFormatterSinkProvider^ serverProv = gcnew BinaryServerFormatterSinkProvider();
      serverProv->TypeFilterLevel = System::Runtime::Serialization::Formatters::TypeFilterLevel::Full;

      BinaryClientFormatterSinkProvider^ clientProv = gcnew BinaryClientFormatterSinkProvider();

      Hashtable^ tcpProps = gcnew Hashtable();
      tcpProps["port"] = "0";
      TcpChannel^ tcpChannel = gcnew TcpChannel(tcpProps, clientProv, serverProv);
      ChannelServices::RegisterChannel(tcpChannel, false);

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
        catch (Exception ^e)
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




    bool PrepareForExport(String^ projectName, String^ projectPath)
    {
      config->Syncxxx();

      config->AddProjectAnyway(projectName, projectPath);

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

      if ((clientProtocol->graphic->Groups == nullptr) || (clientProtocol->graphic->Nodes == nullptr) || 
        (clientProtocol->graphic->Links == nullptr) || (clientProtocol->graphic->Things == nullptr))
        return false;

      //return (!((clientProtocol->graphic->Groups->Count == 0)&&
      //  (clientProtocol->graphic->Nodes->Count == 0)&&
      //  (clientProtocol->graphic->Links->Count == 0)&&
      //  (clientProtocol->graphic->Things->Count == 0)));

      return true;
    };

    void Attach2Scd10()
    {

      AttachProjectForm^ attachProjectForm = gcnew AttachProjectForm();
      if (attachProjectForm->ShowDialog() == DialogResult::OK)
      {
        // Deal with the fact, load the project.
      }
    };

    // ====================================================================
    //
    // ====================================================================

    void AddCreateGroup(__int64 & requestId, LPCSTR GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
      {
      //Guid guid;//Guid(gcnew String(GrpGuid))
      //SysCAD::Protocol::Rectangle^ BR = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      //clientProtocol->CreateGroup(requestId, guid, gcnew String(Tag), gcnew String(Path), BR);
      //GrpGuid = guid.ToString();

      GraphicGroup ^ Grp = gcnew GraphicGroup(Guid(gcnew String(GrpGuid)), gcnew String(Tag));
      //Grp->Tag = gcnew String(Tag);
      Grp->Path = gcnew String(Path);
      Grp->BoundingRect = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      
      m_Create->Add(Grp);
      }


		//void GroupCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect)
  //    {
  //    m_pConn->OnCreateGroup(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
  //      CRectangleF(boundingRect->Left, boundingRect->Top, boundingRect->Width, boundingRect->Height));
  //    }

    // ====================================================================
    //
    // ====================================================================

    void AddCreateNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
      double Angle, const CRectangleF & TagArea, double TagAngle, bool tagVisible, COLORREF FillColor, 
      bool MirrorX, bool MirrorY)
      {
      //  requestId, guid, gcnew String(Tag), gcnew String(Path), 
      //  gcnew String(ClassId), gcnew String(Symbol), BR, Angle, TA, TextAngle, 
      //  Color::Empty, Drawing2D::FillMode::Alternate, MirrorX, MirrorY);
      //ItemGuid = guid.ToString();

      //PKH Comment- Is this correct
      GraphicNode ^ GNd = gcnew GraphicNode(Guid(gcnew String(GraphicGuid)), gcnew String(Tag), gcnew String(Path), Guid(gcnew String(ModelGuid)), 
				gcnew String(Symbol), gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height()),
				0.0, gcnew SysCAD::Protocol::Rectangle(TagArea.Left(), TagArea.Bottom(), TagArea.Width(), TagArea.Height()), TagAngle, tagVisible, Color::Empty, 
				Drawing2D::FillMode::Alternate, MirrorX, MirrorY);

      ModelNode ^ MNd = gcnew ModelNode(Guid(gcnew String(ModelGuid)), gcnew String(Tag), gcnew String(ClassId));

      //List<Item^>  ^Create = gcnew List<Item^>; 
      //List<Item^>  ^Modify;// = gcnew List<Item^>;
      //List<Guid>   ^Delete;// = gcnew List<Guid>;  

      m_Create->Add(MNd);
      m_Create->Add(GNd);

      }

    void DoCreates(__int64 & requestId)//, CString & ModelGuid, CString & GraphicGuid, LPCSTR Tag, LPCSTR Path, 
      {
			////This occurs once only, after the Create list has been fully populated.			
			////Perhaps pass the Create list as a parameter, and call Change in the calling function.
			//bool GOk = clientProtocol->Change(requestId, Create, Modify, Delete);
   //   GraphicGuid = GNd->Guid.ToString();

			////Have moved the Add(MNd) above to the list.
			////Item is parent to GraphicItem and ModelItem.
			////GraphicItem is parent to GraphicNode, ModelItem is parent to ModelNode.
			//// List<Item^> ^Create will contain _all_ created objects.

   //   //PKH Answer - what goes here ?
   //   //bool EOk = engineProtocol->Change(requestId, MCreate, Modify, Delete);
   //   //This has not been implemented yet.
			////At the moment the assumption is the same as it was previously:
			////if an item doens't exist it's ok to create it, and if an item exists it's ok to modify it.			
			//
			//ModelGuid = MNd->Guid.ToString();
      };


    //void ItemCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ shape, SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ tagArea, double tagAngle, System::Drawing::Color fillColor, bool mirrorX, bool mirrorY)
    //  {
    //  m_pConn->OnCreateItem(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
    //    ToCString(model->ToString()), ToCString(shape->ToString()), //boundingRect, 
    //    CRectangleF(boundingRect->Left, boundingRect->Top, boundingRect->Width, boundingRect->Height), 
    //    angle, 
    //    CRectangleF(tagArea->Left, tagArea->Top, tagArea->Width, tagArea->Height),
				//tagAngle,
    //    RGB(fillColor.R, fillColor.G, fillColor.B), 
    //    mirrorX, mirrorY);
    //  }

    // ====================================================================
    //
    // ====================================================================

    void DoDeleteNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid)
      {
      List<Item^>  ^Create;// = gcnew List<Item^>; 
      List<Item^>  ^Modify;// = gcnew List<Item^>;
      List<Guid>   ^GDelete = gcnew List<Guid>;  
      List<Guid>   ^MDelete = gcnew List<Guid>;  

      GDelete->Add(Guid(gcnew String(GraphicGuid)));
      clientProtocol->Change(requestId, Create, Modify, GDelete);

      MDelete->Add(Guid(gcnew String(ModelGuid)));
      //PKH Answer - what goes here ?
      //bool EOk = engineProtocol->Change(requestId, Create, Modify, GDelete
      };

    //void ItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //  {
    //  m_pConn->OnDeleteItem(eventId, requestId, ToCString(guid.ToString()));
    //  }

    // ====================================================================
    //
    // ====================================================================

    //void DoModifyItemPosition(__int64 & requestId, LPCSTR ItemGuid, Pt_3f Delta)
    //  {
    //  //SysCAD::Protocol::Rectangle^ TA(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());
    //  

    //  Guid itemGuid(gcnew String(ItemGuid));
    //  GraphicItem ^ Item;
    //  if (clientProtocol->graphicItems->TryGetValue(itemGuid, Item))
    //    {

    //    SysCAD::Protocol::Rectangle^ BR=Item->BoundingRect;
    //    BR->X+=(float)Delta.X;
    //    BR->Y+=(float)Delta.Y;
    //    //(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());


    //    clientProtocol->ModifyItem(requestId, itemGuid, Item->Tag, Item->Path, 
				//	Item->Model, Item->Shape, BR, Item->Angle, Item->TextArea, Item->TextAngle, 
    //      Color::Empty, Drawing2D::FillMode::Alternate, Item->MirrorX, Item->MirrorY);
    //    }

    //  };

    //void DoModifyItem(__int64 & requestId, LPCSTR ItemGuid, LPCSTR Tag, LPCSTR Path, 
    //  LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect, 
    //  float Angle, const CRectangleF & tagArea, COLORREF FillColor, 
    //  bool MirrorX, bool MirrorY)
    //  {
    //  SysCAD::Protocol::Rectangle^ BR(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
    //  SysCAD::Protocol::Rectangle^ TA(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());
    //  
    //  //clientProtocol->ModifyItem(requestId, guid, gcnew String(Tag), gcnew String(Path), gcnew String(ClassId), gcnew String(Symbol), SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ tagArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    //  clientProtocol->ModifyItem(requestId, Guid(gcnew String(ItemGuid)), gcnew String(Tag), gcnew String(Path), 
    //    gcnew String(ClassId), gcnew String(Symbol), BR, Angle, TA,
    //    Color::Empty, Drawing2D::FillMode::Alternate, MirrorX, MirrorY);
    //    
    //  };

    //void ItemModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ tagArea, double tagAngle, System::Drawing::Color fillColor, bool mirrorX, bool mirrorY)
    //  {
    //  m_pConn->OnModifyItem(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
    //    ToCString(model->ToString()), ToCString(stencil->ToString()), //boundingRect, 
    //    CRectangleF(boundingRect->Left, boundingRect->Top, boundingRect->Width, boundingRect->Height), 
    //    angle, 
    //    CRectangleF(tagArea->Left, tagArea->Top, tagArea->Width, tagArea->Height), 
				//tagAngle,
    //    RGB(fillColor.R, fillColor.G, fillColor.B), 
    //    mirrorX, mirrorY);
    //  }

    //void ItemPathModified(Int64 eventId, Int64 requestId, Guid guid, String^ path)
    //  {
    //  }

    // ====================================================================
    //
    // ====================================================================

    void AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, 
                       LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, 
                       LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
                       LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
                       LPCSTR OriginPort, LPCSTR DestinationPort, 
                       CPointFList & ControlPoints, const CRectangleF & tagArea, double tagAngle, bool tagVisible)
      {
      List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
      POSITION Pos=ControlPoints.GetHeadPosition();
      while (Pos)
        {
        CPointF & Pt=ControlPoints.GetNext(Pos);
        Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
        }

      //SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());

      //m_Creates->As
      //clientProtocol->CreateLink(requestId, gcnew Guid(gcnew String(ModelGuid)), gcnew Guid(gcnew String(GraphicGuid)), gcnew String(Tag), /*gcnew String(Path), */ gcnew String(ClassId), Guid(gcnew String(OriginGuid)), Guid(gcnew String(DestinationGuid)), 
      //  gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, tagAngle);
      //LinkGuid = guid.ToString();

			try
			{

      GraphicLink ^ GLnk = gcnew GraphicLink(Guid(gcnew String(GraphicGuid)), Guid(gcnew String(ModelGuid)), gcnew String(Tag), 
        Guid(gcnew String(OriginGrfGuid)), 0, Guid(gcnew String(DestinationGrfGuid)), 0,  
        Pts, gcnew SysCAD::Protocol::Rectangle(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height()), tagAngle, tagVisible);

      //  gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, tagAngle);
      //LinkGuid = guid.ToString();

      
      
      //GLnk->Tag = gcnew String(Tag);
      //GLnk->Path = gcnew String(Path);
      //GLnk->LinkClass = gcnew String(ClassId);
      //GLnk->Shape = gcnew String(Symbol);
      //GLnk->BoundingRect = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      //GLnk->TextArea = gcnew SysCAD::Protocol::Rectangle(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());
      //GLnk->TextAngle = Angle;
      //GLnk->FillColor = Color::Empty;                  // FillColor ?????
      //GLnk->FillMode = Drawing2D::FillMode::Alternate; // ???
      //GLnk->MirrorX = MirrorX;
      //GLnk->MirrorY = MirrorY;

      ModelLink ^ MLnk = gcnew ModelLink(Guid(gcnew String(ModelGuid)), gcnew String(Tag), gcnew String(ClassId),
        Guid(gcnew String(OriginMdlGuid)), Guid(gcnew String(DestinationMdlGuid)), gcnew String(OriginPort), gcnew String(DestinationPort));
      //MLnk->Tag = gcnew String(Tag);
      //MLnk->LinkClass = gcnew String(ClassId);

      //List<Item^>  ^Create = gcnew List<Item^>; 
      //List<Item^>  ^Modify;// = gcnew List<Item^>;
      //List<Guid>   ^Delete;// = gcnew List<Guid>;  

      m_Create->Add(MLnk);
      m_Create->Add(GLnk);
			}
			catch (Exception ^e)
			{
			}
      };

		//void LinkCreated(Int64 eventId, Int64 requestId, Guid guid, String ^ tag, String ^ classId, Guid origin, Guid destination, 
  //    String ^ originPort, Int16 originPortID, String ^destinationPort, Int16 destinationPortID, List<SysCAD::Protocol::Point^> ^ controlPoints, SysCAD::Protocol::Rectangle^ tagArea, double tagAngle)
  //    {
  //    CPointFList Pts;

  //    for each (SysCAD::Protocol::Point^ Pt in controlPoints)
  //      {
  //      Pts.AddTail(CPointF(Pt->X, Pt->Y));
  //      }

  //    m_pConn->OnCreateLink(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(classId), ToCString(origin.ToString()), ToCString(destination.ToString()), ToCString(originPort), ToCString(destinationPort), 
		//		Pts, CRectangleF(tagArea->Left, tagArea->Top, tagArea->Width, tagArea->Height), tagAngle);
  //    }

    // ====================================================================
    //
    // ====================================================================

    //void DoDeleteLink(__int64 & requestId, LPCSTR LinkGuid)
    //  {
    //  clientProtocol->DeleteLink(requestId, Guid(gcnew String(LinkGuid)));
    //  };

    //void LinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //  {
    //  m_pConn->OnDeleteLink(eventId, requestId, ToCString(guid.ToString()));
    //  }

    // ====================================================================
    //
    // ====================================================================


    //void DoModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, LPCSTR OriginGuid, LPCSTR DestinationGuid, LPCSTR OriginPort, LPCSTR DestinationPort, CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle)
    //  {
    //  List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
    //  POSITION Pos=ControlPoints.GetHeadPosition();
    //  while (Pos)
    //    {
    //    CPointF & Pt=ControlPoints.GetNext(Pos);
    //    Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
    //    }
    //  SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());

    //  clientProtocol->ModifyLink(requestId, Guid(gcnew String(LinkGuid)), gcnew String(Tag), /*LPCSTR Path,*/ gcnew String(ClassId), Guid(gcnew String(OriginGuid)), Guid(gcnew String(DestinationGuid)), 
    //    gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, tagAngle);
    //  };

    //void LinkModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<SysCAD::Protocol::Point^> ^ controlPoints, SysCAD::Protocol::Rectangle^ tagArea, double tagAngle)
    //  {
    //  CPointFList Pts;
    //  for each (SysCAD::Protocol::Point ^ Pt in controlPoints)
    //    {
    //    Pts.AddTail(CPointF(Pt->X, Pt->Y));
    //    }

    //  // Ensure we don't have nulls so the ToString() call succeeds and passes on an empty string in the 
    //  // case where there is no destination or origin port.
    //  if (String::IsNullOrEmpty(originPort))      originPort = String::Empty;
    //  if (String::IsNullOrEmpty(destinationPort)) destinationPort = String::Empty;

    //  m_pConn->OnModifyLink(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(classId), ToCString(origin.ToString()), ToCString(destination.ToString()), ToCString(originPort), ToCString(destinationPort), 
				//Pts, CRectangleF(tagArea->Left, tagArea->Top, tagArea->Width, tagArea->Height), 
				//tagAngle);
    //  }


    // ====================================================================
    //
    // ====================================================================

    //void LinkCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<PointF> controlPoints)
    //  {
    //  }


    void ThingCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect, String^ xaml, double angle, bool mirrorX, bool mirrorY)
      {
      }

    //void LinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //  {
    //  }

    void ThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
      {
      }

    void ThingModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect, String^ xaml, double angle, bool mirrorX, bool mirrorY)
      {
      }

    void ThingPathModified(Int64 eventId, Int64 requestId, Guid guid, String^ path)
      {
      }



    //{

    //Int64 requestId;
    //Guid guid;
    //String ^ tag;
    //String ^ path;
    //String ^ model;
    //String ^ shape;
    //SysCAD::Protocol::Rectangle^ boundingRect;
    //Single angle;
    //System::Drawing::Color fillColor;
    //System::Drawing::Drawing2D::FillMode fillMode;
    //bool mirrorX;
    //bool mirrorY;

    //engineProtocol->ModifyItem(requestId, guid, tag, path, model, shape, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    //}

    bool ProcessChangeLists(Int64 requestId)
      {
      return clientProtocol->Change(requestId, m_Create, m_Modify, m_Delete);
      }


    void PortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String^ tag)
      {
				PortInfo^ portInfo = gcnew PortInfo(PortStatusEnum::Available);
				engineProtocol->RequestPortInfo(requestId, guid, tag, portInfo);
      }

    void Changed(Int64 eventId, Int64 requestId, List<Guid> ^ created, List<Guid> ^ modified, List<Guid> ^ deleted)
      {
			if (created != nullptr)
				//PKH!!! -- this throws an exception: ProcessCreatedList(eventId, requestId, created);

			if (modified != nullptr)
		    ProcessModifiedList(eventId, requestId, modified);

			if (deleted != nullptr)
	      ProcessDeletedList(eventId, requestId, deleted);
      }

    void ProcessCreatedList(Int64 eventId, Int64 requestId, List<Guid> ^ created)
      {
      for each (Guid ^ guid in created )
        {
        ModelNode ^ MNd;
        if (engineProtocol->model->Nodes->TryGetValue(*guid, MNd))
          {
          m_pConn->OnCreateNodeM(eventId, requestId, ToCString(MNd->Guid.ToString()), ToCString(MNd->Tag),  
            ToCString(MNd->NodeClass));
          }

        GraphicNode ^GNd;
        if (clientProtocol->graphic->Nodes->TryGetValue(*guid, GNd))
          {
					ModelNode ^MNd;
					if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
						{
						m_pConn->OnCreateNodeG(eventId, requestId, ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), 
							ToCString(MNd->NodeClass), ToCString(GNd->Shape), //boundingRect, 
							CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), 
							float(GNd->Angle), 
							CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height),
							float(GNd->TagAngle),
							RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), 
							GNd->MirrorX, GNd->MirrorY);
						}
  				}

        ModelLink ^ MLnk;
        if (engineProtocol->model->Links->TryGetValue(*guid, MLnk))
          {
          m_pConn->OnCreateLinkM(eventId, requestId, ToCString(MLnk->Guid.ToString()), ToCString(MLnk->Tag), ToCString(MLnk->LinkClass), 
            ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString()), ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort));
          }

        GraphicLink ^GLnk;
        if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
          {

          CPointFList Pts;
          for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
            {
            Pts.AddTail(CPointF(Pt->X, Pt->Y));
            }
          m_pConn->OnCreateLinkG(eventId, requestId, ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", 
            ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString()), ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort),
            Pts, 
            CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height),
            float(GLnk->TagAngle));
          }
        }
      }

    void ProcessModifiedList(Int64 eventId, Int64 requestId, List<Guid> ^ modified)
      {
      for each (Guid ^ guid in modified)
        {
        ModelNode ^ MNd;
        if (engineProtocol->model->Nodes->TryGetValue(*guid, MNd))
          {
          m_pConn->OnModifyNodeM(eventId, requestId, ToCString(MNd->Guid.ToString()), ToCString(MNd->Tag), ToCString(MNd->NodeClass));
          }

        GraphicNode ^GNd;
        if (clientProtocol->graphic->Nodes->TryGetValue(*guid, GNd))
          {
					ModelNode ^MNd;
					if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
						{
						m_pConn->OnModifyNodeG(eventId, requestId, ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), 
							ToCString(MNd->NodeClass), ToCString(GNd->Shape), //boundingRect, 
							CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), 
							float(GNd->Angle), 
							CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height),
							float(GNd->TagAngle),
							RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), 
							GNd->MirrorX, GNd->MirrorY);
					  }
					}

        ModelLink ^ MLnk;
        if (engineProtocol->model->Links->TryGetValue(*guid, MLnk))
          {
          m_pConn->OnModifyLinkM(eventId, requestId, ToCString(MLnk->Guid.ToString()), ToCString(MLnk->Tag), ToCString(MLnk->LinkClass), 
            ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString()), ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort));
          }

        GraphicLink ^GLnk;
        if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
          {

          CPointFList Pts;
          for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
            {
            Pts.AddTail(CPointF(Pt->X, Pt->Y));
            }
          m_pConn->OnModifyLinkG(eventId, requestId, ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", 
            ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString()), ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort),
            Pts, 
            CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height),
            float(GLnk->TagAngle));
          }
        }
      }

    void ProcessDeletedList(Int64 eventId, Int64 requestId, List<Guid> ^ deleted)
      {
      for each (Guid ^ guid in deleted )
        {
        m_pConn->OnDeleteNodeM(eventId, requestId, ToCString(guid->ToString()));

        m_pConn->OnDeleteNodeG(eventId, requestId, ToCString(guid->ToString()));

        m_pConn->OnDeleteLinkM(eventId, requestId, ToCString(guid->ToString()));

        m_pConn->OnDeleteLinkG(eventId, requestId, ToCString(guid->ToString()));
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

      clientProtocol->LogMessage(requestId, gcnew String(Msg), T);
      };

  protected:
    SysCAD::Protocol::Config ^ config;
    SysCAD::Protocol::ClientProtocol ^ clientProtocol;
    SysCAD::Protocol::EngineProtocol ^ engineProtocol;

    CSvcConnect   * m_pConn;
  
    List<Item^>  ^m_Create;// = gcnew List<Item^>; 
    List<Item^>  ^m_Modify;// = gcnew List<Item^>;
    List<Guid>   ^m_Delete;// = gcnew List<Guid>;  

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

bool CSvcConnectCLR::Startup(CSvcConnect * pConn, LPCSTR configPath)
  {
  String^ configPathString = gcnew String(configPath);

  LogNote("CSvcConnectCLR", 0, "Startup");

  m_pConn =  pConn;
  //m_pSrvr->Initialise();

  CSvcConnectCLRThreadGlbl::gs_SrvrThread = gcnew CSvcConnectCLRThread(m_pConn);//.Startup("");
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->Startup(configPathString);   


  //System::Threading::S
  };

  bool CSvcConnectCLR::PrepareForExport(LPCSTR projectName, LPCSTR projectPath)
  {
    String^ projectNameString = gcnew String(projectName);
    String^ projectPathString = gcnew String(projectPath);
    return CSvcConnectCLRThreadGlbl::gs_SrvrThread->PrepareForExport(projectNameString, projectPathString);   
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
                                  double Angle, const CRectangleF & tagArea, double tagAngle, bool tagVisible, COLORREF FillColor, 
                                  bool MirrorX, bool MirrorY)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateNode(requestId, ModelGuid, GraphicGuid, Tag, Path, 
    ClassId, Symbol, boundingRect, Angle, tagArea, tagAngle, tagVisible, FillColor, MirrorX, MirrorY);
  };

void CSvcConnectCLR::DoDeleteNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoDeleteNode(requestId, ModelGuid, GraphicGuid);
  };

//void CSvcConnectCLR::DoModifyItemPosition(__int64 & requestId, LPCSTR ItemGuid, Pt_3f Delta)
//  {
//  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoModifyItemPosition(requestId, ItemGuid, Delta);
//  }
//
//========================================================================

void CSvcConnectCLR::AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                                  LPCSTR ClassId, 
                                  LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
                                  LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
                                  LPCSTR OriginPort, LPCSTR DestinationPort, 
                                  CPointFList & ControlPoints, const CRectangleF & tagArea, double tagAngle, bool tagVisible)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateLink(requestId, ModelGuid, GraphicGuid, Tag, Path, 
    ClassId, 
    OriginMdlGuid, DestinationMdlGuid, 
    OriginGrfGuid, DestinationGrfGuid, 
    OriginPort, DestinationPort, 
    ControlPoints, tagArea, tagAngle, tagVisible);
  };

//void CSvcConnectCLR::DoDeleteLink(__int64 & requestId, LPCSTR ItemGuid)
//  {
//  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoDeleteLink(requestId, ItemGuid);
//  };
//
//
//void CSvcConnectCLR::DoModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, 
//                                  LPCSTR ClassId, 
//                                  LPCSTR OriginGuid, LPCSTR DestinationGuid, 
//                                  LPCSTR OriginPort, LPCSTR DestinationPort, 
//                                  CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle)
//  {
//  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoModifyLink(requestId, LinkGuid, Tag, Path, 
//    ClassId, 
//    OriginGuid, DestinationGuid, OriginPort, DestinationPort, 
//    ControlPoints, tagArea, tagAngle);
//  };
//

//========================================================================

bool CSvcConnectCLR::ProcessChangeLists(__int64 & requestId)
  {
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->ProcessChangeLists(requestId);
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
