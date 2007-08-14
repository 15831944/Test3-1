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
      m_DoingExport=false;  
      };
    ~CSvcConnectCLRThread()
      {
      };

    bool Startup(String^ projectPath, String^ configPath, bool ImportScd9)
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
          clientProtocol->Connect();
          engineProtocol->Connect();

          /*
          This is where import goes
          */

          clientProtocol->GroupCreated += gcnew ClientProtocol::GroupCreatedHandler(this, &CSvcConnectCLRThread::GroupCreated);
          clientProtocol->ItemCreated  += gcnew ClientProtocol::ItemCreatedHandler(this, &CSvcConnectCLRThread::ItemCreated);
          //clientProtocol->ItemModified += gcnew EngineProtocol::ItemModifiedHandler(this, &CSvcConnectCLRThread::ItemModified);
          clientProtocol->ItemDeleted  += gcnew ClientProtocol::ItemDeletedHandler(this, &CSvcConnectCLRThread::ItemDeleted);

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

    void DoCreateGroup(__int64 & requestId, CString & GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
      {
      Guid guid;//Guid(gcnew String(GrpGuid))
      RectangleF BR(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      clientProtocol->CreateGroup(requestId, guid, gcnew String(Tag), gcnew String(Path), BR);
      GrpGuid = guid.ToString();
      }


    void GroupCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, RectangleF boundingRect)
      {
      m_pConn->OnCreateGroup(m_DoingExport, eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
        CRectangleF(boundingRect.Left, boundingRect.Top, boundingRect.Width, boundingRect.Height));
      }

    // ====================================================================

    void DoCreateItem(__int64 & requestId, CString & ItemGuid, LPCSTR Tag, LPCSTR Path, 
                                      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                                      float Angle, const CRectangleF & textArea, COLORREF FillColor, 
                                      bool MirrorX, bool MirrorY)
      {
      Guid guid;//X(gcnew String(guid))
      RectangleF BR(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
      RectangleF TA(textArea.Left(), textArea.Bottom(), textArea.Width(), textArea.Height());

      clientProtocol->CreateItem(requestId, guid, gcnew String(Tag), gcnew String(Path), 
        gcnew String(ClassId), gcnew String(Symbol), BR, Angle, TA,
		Color::Black, Drawing2D::FillMode::Alternate, MirrorX, MirrorY);
      ItemGuid = guid.ToString();
      };

    void ItemCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ shape, RectangleF boundingRect, Single angle, RectangleF textArea, System::Drawing::Color fillColor, bool mirrorX, bool mirrorY)
      {
      m_pConn->OnCreateItem(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
        ToCString(model->ToString()), ToCString(shape->ToString()), //boundingRect, 
        CRectangleF(boundingRect.Left, boundingRect.Top, boundingRect.Width, boundingRect.Height), 
        angle, 
	    CRectangleF(textArea.Left, textArea.Top, textArea.Width, textArea.Height),
		RGB(fillColor.R, fillColor.G, fillColor.B), 
        mirrorX, mirrorY);
      }

    // ====================================================================

    void ItemModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System::Drawing::Color fillColor, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      m_pConn->OnModifyItem(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
        ToCString(model->ToString()), ToCString(stencil->ToString()), //boundingRect, 
        CRectangleF(boundingRect.Left, boundingRect.Top, boundingRect.Width, boundingRect.Height), 
        angle, 
		CRectangleF(textArea.Left, textArea.Top, textArea.Width, textArea.Height), 
		RGB(fillColor.R, fillColor.G, fillColor.B), 
        mirrorX, mirrorY);
      }

    void ItemPathModified(Int64 eventId, Int64 requestId, Guid guid, String^ path)
      {
      }

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

    void LinkCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<PointF> controlPoints)
      {
      }

    void ThingCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, RectangleF boundingRect, String^ xaml, Single angle, bool mirrorX, bool mirrorY)
      {
      }

    void LinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
      {
      }

    void ThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
      {
      }

    void LinkModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<PointF> controlPoints)
      {
      }

    void ThingModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, RectangleF boundingRect, String^ xaml, Single angle, bool mirrorX, bool mirrorY)
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
      //RectangleF boundingRect;
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

    void Export(String ^ filename)
      {
      // Recreates Graphics but leaves the models in place

      m_DoingExport=true;

      //CGetExistingItems GI;

      //int PrevPage=-1;

      //static __int64 RqID=0;

      //while (GI.GetOne())
      //  {
      //  CGrfTagInfo & I = GI.Item();

      //  dbgpln("Export Item/Link %i %-20s %-20s %-20s", GI.Type(), I.m_sTag(), I.m_sSymbol(), I.m_sClass());
      //  
      //  System::Guid guid(gcnew String(GI.Guid()));
      //  guid=System::Guid::NewGuid();
      //  String ^ page =  gcnew String(GI.PageName());//"Pg1";

      //  // Simple Layout
      //  int NAcross=Max(1,int(Sqrt((double)GI.PageCount())+0.5));
      //  float XOffSet=(GI.PageNo()%NAcross)*310.0f*1.414f;
      //  float YOffSet=(GI.PageNo()/NAcross)*310.0f;

      //  if (PrevPage!=GI.PageNo())
      //    {
      //    PrevPage=GI.PageNo();

      //    String ^ path = "/" + filename + "/";
      //    engineProtocol->CreateGroup(RqID++, guid, gcnew String(GI.PageName()), gcnew String(path), 
      //      RectangleF(float(GI.PageRct().Left()+XOffSet),float(GI.PageRct().Bottom()+YOffSet), float(GI.PageRct().Width()), float(GI.PageRct().Height())));
      //    }

      //  switch (GI.Type())
      //    {
      //    case CGetExistingItems::eIsNode:
      //      {
      //      String ^ path = "/" + filename + "/" + page + "/";

      //      CString S(I.m_sSymbol());
      //      int iDollar=S.Find('$');
      //      if (iDollar>=0)
      //        S.Delete(0, iDollar+1);

      //      Model ^ model = gcnew Model(gcnew String(I.m_sClass()));
      //      Shape ^ shape = gcnew Shape(gcnew String(I.m_sClass()));

      //      engineProtocol->CreateItem(RqID++, guid, gcnew String(I.m_sTag()),
      //        path, model, shape,
      //        RectangleF(float(I.m_LoBnd.m_X+XOffSet), float((GI.PageRct().Height()-I.m_HiBnd.m_Y+YOffSet)), float(I.m_HiBnd.m_X-I.m_LoBnd.m_X), float(I.m_HiBnd.m_Y-I.m_LoBnd.m_Y)),
      //        0.0, Color(), Drawing2D::FillMode()  , false, false);
      //      break;
      //      }

      //    case CGetExistingItems::eIsLink:
      //      {
      //      break;
      //      }
      //    }
      //  }

      m_DoingExport=false;
      }

  protected:
    SysCAD::Protocol::Config ^ config;
    SysCAD::Protocol::ClientProtocol ^ clientProtocol;
    SysCAD::Protocol::EngineProtocol ^ engineProtocol;
    
    CSvcConnect   * m_pConn;
    bool            m_DoingExport;
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

bool CSvcConnectCLR::Startup(CSvcConnect * pConn, LPCSTR projectPath, LPCSTR configPath, bool ImportScd9)
  {
  String^ projectPathString = gcnew String(projectPath);
  String^ configPathString = gcnew String(configPath);

  LogNote("CSvcConnectCLR", 0, "Startup");

  m_pConn =  pConn;
  //m_pSrvr->Initialise();
  
  CSvcConnectCLRThreadGlbl::gs_SrvrThread = gcnew CSvcConnectCLRThread(m_pConn);//.Startup("");
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->Startup(projectPathString, configPathString, ImportScd9);   


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
                                  float Angle, const CRectangleF & textArea, COLORREF FillColor, 
                                  bool MirrorX, bool MirrorY)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoCreateItem(requestId, ItemGuid, Tag, Path, 
                                  ClassId, Symbol, boundingRect, Angle, textArea, FillColor, MirrorX, MirrorY);
  };

void CSvcConnectCLR::DoDeleteItem(__int64 & requestId, LPCSTR ItemGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->DoDeleteItem(requestId, ItemGuid);
  };

//========================================================================

void CSvcConnectCLR::Export(LPCSTR projectPath, LPCSTR configPath)
  {
  String^ projectPathString = gcnew String(projectPath);
  String^ configPathString = gcnew String(configPath);

  LogNote("CSvcConnectCLR", 0, "Import");

  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Export(projectPathString);   

  //System::Threading::S
  };

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
