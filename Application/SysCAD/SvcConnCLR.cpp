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
      };
    ~CSvcConnectCLRThread()
      {
      };

    bool Startup(String^ projectPath, String^ configPath)
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
          proc->StartInfo->Arguments = "\" " + projectPath + " \" \" " + configPath + " \" \" " + stencilPath + " \"";
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
      if (success)
        {
        config->Syncxxx();


        bool clientSuccess = false;
        bool engineSuccess = false;
        i=0;
        while ((!((clientSuccess)&&(engineSuccess)))&&(i++ < 20)) //_MUST_ find a better way to handle this! (but only temporary...)
          {
          // Basically need to wait until service is ready.
          Sleep(i*i*i); // Last wait will be 1sec.

          delete clientProtocol;
          delete engineProtocol;
          clientProtocol = gcnew ClientProtocol;
          engineProtocol = gcnew EngineProtocol;

          // Connect to graphic data.

          clientSuccess = clientProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Client/" + 
            Path::GetFileNameWithoutExtension(Path::GetDirectoryName(projectPath))));
          engineSuccess = engineProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Engine/" + 
            Path::GetFileNameWithoutExtension(Path::GetDirectoryName(projectPath))));
          }
        if (clientSuccess&&engineSuccess)
          {
          clientProtocol->Connect("Engine as Client");
          engineProtocol->Connect("Engine as Engine");

          // This will allow the editor to create/delete in addition to modify after the project loads.
          __int64 requestId;
          engineProtocol->ChangeState(requestId, SysCAD::Protocol::EngineBaseProtocol::RunState::Edit);

					engineProtocol->PortInfoRequested += gcnew EngineProtocol::PortInfoRequestedHandler(this, &CSvcConnectCLRThread::PortInfoRequested);

          /*
          This is where import goes
          */

          clientProtocol->GroupCreated += gcnew ClientProtocol::GroupCreatedHandler(this, &CSvcConnectCLRThread::GroupCreated);

          clientProtocol->ItemCreated  += gcnew ClientProtocol::ItemCreatedHandler(this, &CSvcConnectCLRThread::ItemCreated);
          clientProtocol->ItemModified += gcnew ClientProtocol::ItemModifiedHandler(this, &CSvcConnectCLRThread::ItemModified);
          clientProtocol->ItemDeleted  += gcnew ClientProtocol::ItemDeletedHandler(this, &CSvcConnectCLRThread::ItemDeleted);

          clientProtocol->LinkCreated  += gcnew ClientProtocol::LinkCreatedHandler(this, &CSvcConnectCLRThread::LinkCreated);
          clientProtocol->LinkModified += gcnew ClientProtocol::LinkModifiedHandler(this, &CSvcConnectCLRThread::LinkModified);
          clientProtocol->LinkDeleted  += gcnew ClientProtocol::LinkDeletedHandler(this, &CSvcConnectCLRThread::LinkDeleted);

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
        }
      if (clientProtocol == nullptr)
        return false;

      if ((clientProtocol->graphicGroups == nullptr) || (clientProtocol->graphicItems == nullptr) || 
        (clientProtocol->graphicLinks == nullptr) || (clientProtocol->graphicThings == nullptr))
        return false;

      return (!((clientProtocol->graphicGroups->Count == 0)&&
        (clientProtocol->graphicItems->Count == 0)&&
        (clientProtocol->graphicLinks->Count == 0)&&
        (clientProtocol->graphicThings->Count == 0)));
      };

    // ====================================================================
    //
    // ====================================================================

    void DoCreateGroup(__int64 & requestId, CString & GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
      {
      Guid guid;//Guid(gcnew String(GrpGuid))
      SysCAD::Protocol::Rectangle^ BR = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      clientProtocol->CreateGroup(requestId, guid, gcnew String(Tag), gcnew String(Path), BR);
      GrpGuid = guid.ToString();
      }


		void GroupCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect)
      {
      m_pConn->OnCreateGroup(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
        CRectangleF(boundingRect->Left, boundingRect->Top, boundingRect->Width, boundingRect->Height));
      }

    // ====================================================================
    //
    // ====================================================================

    void DoCreateItem(__int64 & requestId, CString & ItemGuid, LPCSTR Tag, LPCSTR Path, 
      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
      float Angle, const CRectangleF & textArea, float TextAngle, COLORREF FillColor, 
      bool MirrorX, bool MirrorY)
      {
      Guid guid;//X(gcnew String(guid))
      SysCAD::Protocol::Rectangle^ BR = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(textArea.Left(), textArea.Bottom(), textArea.Width(), textArea.Height());

      bool a = clientProtocol->CreateItem(requestId, guid, gcnew String(Tag), gcnew String(Path), 
        gcnew String(ClassId), gcnew String(Symbol), BR, Angle, TA, TextAngle, 
        Color::Empty, Drawing2D::FillMode::Alternate, MirrorX, MirrorY);
      ItemGuid = guid.ToString();
      };

    void PortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String^ tag)
      {
				PortInfo^ portInfo = gcnew PortInfo(PortStatusEnum::Available);
				engineProtocol->RequestPortInfo(requestId, guid, tag, portInfo);
      }

    void ItemCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ shape, SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ textArea, double textAngle, System::Drawing::Color fillColor, bool mirrorX, bool mirrorY)
      {
      m_pConn->OnCreateItem(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
        ToCString(model->ToString()), ToCString(shape->ToString()), //boundingRect, 
        CRectangleF(boundingRect->Left, boundingRect->Top, boundingRect->Width, boundingRect->Height), 
        angle, 
        CRectangleF(textArea->Left, textArea->Top, textArea->Width, textArea->Height),
				textAngle,
        RGB(fillColor.R, fillColor.G, fillColor.B), 
        mirrorX, mirrorY);
      }

    // ====================================================================
    //
    // ====================================================================

    void DoDeleteItem(__int64 & requestId, LPCSTR ItemGuid)
      {
      clientProtocol->DeleteItem(requestId, Guid(gcnew String(ItemGuid)));
      };

    void ItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
      {
      m_pConn->OnDeleteItem(eventId, requestId, ToCString(guid.ToString()));
      }

    // ====================================================================
    //
    // ====================================================================

    void DoModifyItemPosition(__int64 & requestId, LPCSTR ItemGuid, Pt_3f Delta)
      {
      //SysCAD::Protocol::Rectangle^ TA(textArea.Left(), textArea.Bottom(), textArea.Width(), textArea.Height());
      

      Guid itemGuid(gcnew String(ItemGuid));
      GraphicItem ^ Item;
      if (clientProtocol->graphicItems->TryGetValue(itemGuid, Item))
        {

        SysCAD::Protocol::Rectangle^ BR=Item->BoundingRect;
        BR->X+=(float)Delta.X;
        BR->Y+=(float)Delta.Y;
        //(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());


        clientProtocol->ModifyItem(requestId, itemGuid, Item->Tag, Item->Path, 
					Item->Model, Item->Shape, BR, Item->Angle, Item->TextArea, Item->TextAngle, 
          Color::Empty, Drawing2D::FillMode::Alternate, Item->MirrorX, Item->MirrorY);
        }

      };

    //void DoModifyItem(__int64 & requestId, LPCSTR ItemGuid, LPCSTR Tag, LPCSTR Path, 
    //  LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect, 
    //  float Angle, const CRectangleF & textArea, COLORREF FillColor, 
    //  bool MirrorX, bool MirrorY)
    //  {
    //  SysCAD::Protocol::Rectangle^ BR(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
    //  SysCAD::Protocol::Rectangle^ TA(textArea.Left(), textArea.Bottom(), textArea.Width(), textArea.Height());
    //  
    //  //clientProtocol->ModifyItem(requestId, guid, gcnew String(Tag), gcnew String(Path), gcnew String(ClassId), gcnew String(Symbol), SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ textArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    //  clientProtocol->ModifyItem(requestId, Guid(gcnew String(ItemGuid)), gcnew String(Tag), gcnew String(Path), 
    //    gcnew String(ClassId), gcnew String(Symbol), BR, Angle, TA,
    //    Color::Empty, Drawing2D::FillMode::Alternate, MirrorX, MirrorY);
    //    
    //  };

    void ItemModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ textArea, double textAngle, System::Drawing::Color fillColor, bool mirrorX, bool mirrorY)
      {
      m_pConn->OnModifyItem(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
        ToCString(model->ToString()), ToCString(stencil->ToString()), //boundingRect, 
        CRectangleF(boundingRect->Left, boundingRect->Top, boundingRect->Width, boundingRect->Height), 
        angle, 
        CRectangleF(textArea->Left, textArea->Top, textArea->Width, textArea->Height), 
				textAngle,
        RGB(fillColor.R, fillColor.G, fillColor.B), 
        mirrorX, mirrorY);
      }

    void ItemPathModified(Int64 eventId, Int64 requestId, Guid guid, String^ path)
      {
      }

    // ====================================================================
    //
    // ====================================================================

    void DoCreateLink(__int64 & requestId, CString & LinkGuid, LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, LPCSTR OriginGuid, LPCSTR DestinationGuid, LPCSTR OriginPort, LPCSTR DestinationPort, CPointFList & ControlPoints, const CRectangleF & textArea, float textAngle)
      {
      List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
      POSITION Pos=ControlPoints.GetHeadPosition();
      while (Pos)
        {
        CPointF & Pt=ControlPoints.GetNext(Pos);
        Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
        }

      SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(textArea.Left(), textArea.Bottom(), textArea.Width(), textArea.Height());

      Guid guid;
      clientProtocol->CreateLink(requestId, guid, gcnew String(Tag), /*gcnew String(Path), */ gcnew String(ClassId), Guid(gcnew String(OriginGuid)), Guid(gcnew String(DestinationGuid)), 
        gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, textAngle);
      LinkGuid = guid.ToString();
      };

		void LinkCreated(Int64 eventId, Int64 requestId, Guid guid, String ^ tag, String ^ classId, Guid origin, Guid destination, 
      String ^ originPort, Int16 originPortID, String ^destinationPort, Int16 destinationPortID, List<SysCAD::Protocol::Point^> ^ controlPoints, SysCAD::Protocol::Rectangle^ textArea, double textAngle)
      {
      CPointFList Pts;

      for each (SysCAD::Protocol::Point^ Pt in controlPoints)
        {
        Pts.AddTail(CPointF(Pt->X, Pt->Y));
        }

      m_pConn->OnCreateLink(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(classId), ToCString(origin.ToString()), ToCString(destination.ToString()), ToCString(originPort), ToCString(destinationPort), 
				Pts, CRectangleF(textArea->Left, textArea->Top, textArea->Width, textArea->Height), textAngle);
      }

    // ====================================================================
    //
    // ====================================================================

    void DoDeleteLink(__int64 & requestId, LPCSTR LinkGuid)
      {
      clientProtocol->DeleteLink(requestId, Guid(gcnew String(LinkGuid)));
      };

    void LinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
      {
      m_pConn->OnDeleteLink(eventId, requestId, ToCString(guid.ToString()));
      }

    // ====================================================================
    //
    // ====================================================================


    void DoModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, LPCSTR OriginGuid, LPCSTR DestinationGuid, LPCSTR OriginPort, LPCSTR DestinationPort, CPointFList & ControlPoints, const CRectangleF & textArea, float textAngle)
      {
      List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
      POSITION Pos=ControlPoints.GetHeadPosition();
      while (Pos)
        {
        CPointF & Pt=ControlPoints.GetNext(Pos);
        Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
        }
      SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(textArea.Left(), textArea.Bottom(), textArea.Width(), textArea.Height());

      clientProtocol->ModifyLink(requestId, Guid(gcnew String(LinkGuid)), gcnew String(Tag), /*LPCSTR Path,*/ gcnew String(ClassId), Guid(gcnew String(OriginGuid)), Guid(gcnew String(DestinationGuid)), 
        gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, textAngle);
      };

    void LinkModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<SysCAD::Protocol::Point^> ^ controlPoints, SysCAD::Protocol::Rectangle^ textArea, double textAngle)
      {
      CPointFList Pts;
      for each (SysCAD::Protocol::Point ^ Pt in controlPoints)
        {
        Pts.AddTail(CPointF(Pt->X, Pt->Y));
        }

      // Ensure we don't have nulls so the ToString() call succeeds and passes on an empty string in the 
      // case where there is no destination or origin port.
      if (String::IsNullOrEmpty(originPort))      originPort = String::Empty;
      if (String::IsNullOrEmpty(destinationPort)) destinationPort = String::Empty;

      m_pConn->OnModifyLink(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(classId), ToCString(origin.ToString()), ToCString(destination.ToString()), ToCString(originPort), ToCString(destinationPort), 
				Pts, CRectangleF(textArea->Left, textArea->Top, textArea->Width, textArea->Height), 
				textAngle);
      }


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

  protected:
    SysCAD::Protocol::Config ^ config;
    SysCAD::Protocol::ClientProtocol ^ clientProtocol;
    SysCAD::Protocol::EngineProtocol ^ engineProtocol;

    CSvcConnect   * m_pConn;
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

bool CSvcConnectCLR::Startup(CSvcConnect * pConn, LPCSTR projectPath, LPCSTR configPath)
  {
  String^ projectPathString = gcnew String(projectPath);
  String^ configPathString = gcnew String(configPath);

  LogNote("CSvcConnectCLR", 0, "Startup");

  m_pConn =  pConn;
  //m_pSrvr->Initialise();

  CSvcConnectCLRThreadGlbl::gs_SrvrThread = gcnew CSvcConnectCLRThread(m_pConn);//.Startup("");
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->Startup(projectPathString, configPathString);   


  //System::Threading::S
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

void CSvcConnectCLR::DoCreateGroup(__int64 & requestId, CString & GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoCreateGroup(requestId, GrpGuid, Tag, Path, boundingRect);
  };

//========================================================================

void CSvcConnectCLR::DoCreateItem(__int64 & requestId, CString & ItemGuid, LPCSTR Tag, LPCSTR Path,  
                                  LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                                  float Angle, const CRectangleF & textArea, float textAngle, COLORREF FillColor, 
                                  bool MirrorX, bool MirrorY)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoCreateItem(requestId, ItemGuid, Tag, Path, 
    ClassId, Symbol, boundingRect, Angle, textArea, textAngle, FillColor, MirrorX, MirrorY);
  };

void CSvcConnectCLR::DoDeleteItem(__int64 & requestId, LPCSTR ItemGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoDeleteItem(requestId, ItemGuid);
  };

void CSvcConnectCLR::DoModifyItemPosition(__int64 & requestId, LPCSTR ItemGuid, Pt_3f Delta)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoModifyItemPosition(requestId, ItemGuid, Delta);
  }

//========================================================================

void CSvcConnectCLR::DoCreateLink(__int64 & requestId, CString & LinkGuid, LPCSTR Tag, LPCSTR Path, 
                                  LPCSTR ClassId, 
                                  LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                  LPCSTR OriginPort, LPCSTR DestinationPort, 
                                  CPointFList & ControlPoints, const CRectangleF & textArea, float textAngle)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoCreateLink(requestId, LinkGuid, Tag, Path, 
    ClassId, 
    OriginGuid, DestinationGuid, 
    OriginPort, DestinationPort, 
    ControlPoints, textArea, textAngle);
  };

void CSvcConnectCLR::DoDeleteLink(__int64 & requestId, LPCSTR ItemGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoDeleteLink(requestId, ItemGuid);
  };


void CSvcConnectCLR::DoModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, 
                                  LPCSTR ClassId, 
                                  LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                                  LPCSTR OriginPort, LPCSTR DestinationPort, 
                                  CPointFList & ControlPoints, const CRectangleF & textArea, float textAngle)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoModifyLink(requestId, LinkGuid, Tag, Path, 
    ClassId, 
    OriginGuid, DestinationGuid, OriginPort, DestinationPort, 
    ControlPoints, textArea, textAngle);
  };

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

#endif

//========================================================================
