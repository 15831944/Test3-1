#include <stdafx.h> //pkh
#include <afxwin.h>         // MFC core and standard components
#include "..\..\common\scd\scdlib\gpfuncs.h"
#include "errorlog.h"

#include "NETServer.h"
#include "NETServerU.h"

//========================================================================

#if CREATENETSERVER

//========================================================================

using namespace System;
using namespace System::IO;
//using namespace System::IO::Directory;

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

      Hashtable ^ ipcProps = gcnew Hashtable();
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

                    ////Create dummy graphicstencil for comparison...
                    //{
                    //  ModelStencil^ modelStencil = gcnew ModelStencil();
                    //  modelStencil->Tag = "";
                    //  modelStencil->GroupName = "Control";
                    //  ArrayList^ elements = gcnew ArrayList();
                    //  SysCAD::Interface::Arc^ arc = gcnew SysCAD::Interface::Arc(0, 0, 100, 100, 10, 360);
                    //  elements->Add(arc);
                    //  modelStencil->Elements = elements;

                    //  modelStencil->Decorations = gcnew ArrayList();
                    //  modelStencil->Anchors = gcnew ArrayList();
                    //  modelStencil->FillMode = System::Drawing::Drawing2D::FillMode::Alternate;

                    //  SoapFormatter^ sf = gcnew SoapFormatter();
                    //  StreamWriter ^ streamWriter = gcnew StreamWriter(fullpath+".new");
                    //  Stream^ stream = streamWriter->BaseStream;
                    //  sf->Serialize(stream, modelStencil);
                    //  stream->Close();
                    //}

        SoapFormatter ^ sf = gcnew SoapFormatter;
        StreamReader ^ streamRdr = gcnew StreamReader(fullpath);
        Stream ^ stream = streamRdr->BaseStream;
        ModelStencil ^ modelStencil = (ModelStencil^)sf->Deserialize(stream);
        modelStencil->Tag = Path::GetFileNameWithoutExtension(fullpath);
        m_Config->ModelStencils->Add(Path::GetFileNameWithoutExtension(fullpath), modelStencil);
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

                    ////Create dummy graphicstencil for comparison...
                    //{
                    //  GraphicStencil^ graphicStencil = gcnew GraphicStencil();
                    //  graphicStencil->Tag = "";
                    //  ArrayList^ elements = gcnew ArrayList();
                    //  SysCAD::Interface::Arc^ arc = gcnew SysCAD::Interface::Arc(0, 0, 100, 100, 10, 360);
                    //  elements->Add(arc);
                    //  graphicStencil->Elements = elements;

                    //  graphicStencil->Decorations = gcnew ArrayList();

                    //  SoapFormatter^ sf = gcnew SoapFormatter();
                    //  StreamWriter ^ streamWriter = gcnew StreamWriter(fullpath+".new");
                    //  Stream^ stream = streamWriter->BaseStream;
                    //  sf->Serialize(stream, graphicStencil);
                    //  stream->Close();
                    //}

        SoapFormatter ^ sf = gcnew SoapFormatter;
        Stream ^ stream = (gcnew StreamReader(fullpath))->BaseStream;
        GraphicStencil ^ graphicStencil = (GraphicStencil^)sf->Deserialize(stream);
        stream->Close();


        graphicStencil->Tag = Path::GetFileNameWithoutExtension(fullpath);
        m_Config->GraphicStencils->Add(Path::GetFileNameWithoutExtension(fullpath), graphicStencil);
        //Console::WriteLine("  {0}] {1}", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        }
      }

    void MarshalConfig()
      {
      RemotingServices::Marshal(m_Config, "Global");
      }

    bool ChangeState(ServiceGraphic^ graphic, Int64 requestID, BaseGraphic::RunStates runState)
      {
      if (true) // Decide whether to allow runstate change
        { // We're going to do it.
        // Change the runstate.

        // Raise event(s).
        graphic->DoStateChanged(requestID, runState);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    void GetTagValues(ServiceGraphic^ graphic, Int64 requestID, ArrayList^% tagList)
      {
        // Return modified ArrayList with tag details included.
      }

    bool CreateItem(ServiceGraphic^ graphic, Int64 requestID, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to create an item.
        { // We're going to do it.
        // Create the item.

        // Raise event(s).
        graphic->DoItemCreated(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool ModifyItem(ServiceGraphic^ graphic, Int64 requestID, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to modify an item.
        { // We're going to do it.
        // Modify the item.

        // Raise event(s).
       graphic->DoItemModified(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool DeleteItem(ServiceGraphic^ graphic, Int64 requestID, Guid guid)
      {
      if (true) // Decide whether to delete an item.
        { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        graphic->DoItemDeleted(requestID, guid);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool CreateLink(ServiceGraphic^ graphic, Int64 requestID, Guid guid, String^ tag, String^ classID, Guid origin, Guid destination, String^ originPort, String^ destinationPort, Generic::List<PointF>^ controlPoints)
      {
      if (true) // Decide whether to create an link.
        { // We're going to do it.
        // Create the item.

        // Raise event(s).
        graphic->DoLinkCreated(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool ModifyLink(ServiceGraphic^ graphic, Int64 requestID, Guid guid, String^ tag, String^ classID, Guid origin, Guid destination, String^ originPort, String^ destinationPort, Generic::List<PointF>^ controlPoints)
      {
      if (true) // Decide whether to modify an link.
        { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        graphic->DoLinkModified(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool DeleteLink(ServiceGraphic^ graphic, Int64 requestID, Guid guid)
      {
      if (true) // Decide whether to delete an link.
        { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        graphic->DoLinkDeleted(requestID, guid);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool CreateThing(ServiceGraphic^ graphic, Int64 requestID, Guid guid, String^ tag, String^ path, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, ArrayList^ elements, ArrayList^ decorations, ArrayList^ textArea, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to create an Thing.
        { // We're going to do it.
        // Create the Thing.

        // Raise event(s).
        graphic->DoThingCreated(requestID, guid, tag, path, boundingRect, angle, fillColor, elements, decorations, textArea, fillMode, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool ModifyThing(ServiceGraphic^ graphic, Int64 requestID, Guid guid, String^ tag, String^ path, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, ArrayList^ elements, ArrayList^ decorations, ArrayList^ textArea, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to modify an Thing.
        { // We're going to do it.
        // Modify the Thing.

        // Raise event(s).
       graphic->DoThingModified(requestID, guid, tag, path, boundingRect, angle, fillColor, elements, decorations, textArea, fillMode, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool DeleteThing(ServiceGraphic^ graphic, Int64 requestID, Guid guid)
      {
      if (true) // Decide whether to delete an Thing.
        { // We're going to do it.
        // Delete the Thing.

        // Raise event(s).
        graphic->DoThingDeleted(requestID, guid);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    PortStatus PortCheck(ServiceGraphic^ graphic, Guid guid, Anchor^ anchor)
      {
      //		CNSGuidItem * pGuid = new CNSGuidItem();
      //		pGuid->m_Guid = guid;
      //		m_pUnmanaged->m_Guids.AddTail(null);
      //		CNSGuidItem * pGuid = m_pUnmanaged->m_Guids.Find(guid);
      //		CNSMdlLink * pLink = dynamic_cast<CNSMdlLink *>(guid);
      return PortStatus::Available;
      }

    ArrayList^ PropertyListCheck(ServiceGraphic^ graphic, Guid guid, String^ tag, String^ path)
      {
        char* dest = new char[tag->Length+1];
        strcpy(dest, static_cast<LPCTSTR>(const_cast<void*>(static_cast<const void*>(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(tag)))));
        ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)dest);

        ArrayList^ list = gcnew ArrayList();
        // Generate list of properties at this level in properties tree.
        return list;
      }

    void MarshalGraphics()
    {
      ServiceGraphic::ChangeStateHandler^ changeState = gcnew ServiceGraphic::ChangeStateHandler(this, &CNETServerThread::ChangeState);

      ServiceGraphic::GetTagValuesHandler^ getTagValues = gcnew ServiceGraphic::GetTagValuesHandler(this, &CNETServerThread::GetTagValues);

      ServiceGraphic::CreateItemHandler^ createItem = gcnew ServiceGraphic::CreateItemHandler(this, &CNETServerThread::CreateItem);
      ServiceGraphic::ModifyItemHandler^ modifyItem = gcnew ServiceGraphic::ModifyItemHandler(this, &CNETServerThread::ModifyItem);
      ServiceGraphic::DeleteItemHandler^ deleteItem = gcnew ServiceGraphic::DeleteItemHandler(this, &CNETServerThread::DeleteItem);

      ServiceGraphic::CreateLinkHandler^ createLink = gcnew ServiceGraphic::CreateLinkHandler(this, &CNETServerThread::CreateLink);
      ServiceGraphic::ModifyLinkHandler^ modifyLink = gcnew ServiceGraphic::ModifyLinkHandler(this, &CNETServerThread::ModifyLink);
      ServiceGraphic::DeleteLinkHandler^ deleteLink = gcnew ServiceGraphic::DeleteLinkHandler(this, &CNETServerThread::DeleteLink);

      ServiceGraphic::CreateThingHandler^ createThing = gcnew ServiceGraphic::CreateThingHandler(this, &CNETServerThread::CreateThing);
      ServiceGraphic::ModifyThingHandler^ modifyThing = gcnew ServiceGraphic::ModifyThingHandler(this, &CNETServerThread::ModifyThing);
      ServiceGraphic::DeleteThingHandler^ deleteThing = gcnew ServiceGraphic::DeleteThingHandler(this, &CNETServerThread::DeleteThing);

      ServiceGraphic::PortCheckHandler^ portCheck = gcnew ServiceGraphic::PortCheckHandler(this, &CNETServerThread::PortCheck);

      ServiceGraphic::PropertyListHandler^ propertyListCheck = gcnew ServiceGraphic::PropertyListHandler(this, &CNETServerThread::PropertyListCheck);


      ServiceGraphic ^ graphic = gcnew ServiceGraphic(changeState, getTagValues, createItem, modifyItem, deleteItem, createLink, modifyLink, deleteLink, createThing, modifyThing, deleteThing, portCheck, propertyListCheck);

      String ^ filename;
      filename = gcnew String(m_pUnmanaged->m_PrjName);

      StringCollection ^ pages = gcnew StringCollection();
      {
        {
          POSITION Pos=m_pUnmanaged->m_Guids.GetHeadPosition();
          while (Pos)
          {
            CNSGuidItem * pGuid = m_pUnmanaged->m_Guids.GetNext(Pos);
            if (!pGuid->m_IsLink)
            {
              CNSMdlNode * pNode = dynamic_cast<CNSMdlNode *>(pGuid);
              String ^ page = gcnew String(pNode->m_pGrfs[0]->m_Page);
              if (!pages->Contains(page))
              {
                pages->Add(page);
              }
            }
          }
        }

        {
          POSITION Pos=m_pUnmanaged->m_Things.GetHeadPosition();
          while (Pos)
          {
            CNSGrfThing * pThing = m_pUnmanaged->m_Things.GetNext(Pos);
            String ^ page = gcnew String(pThing->m_Page);
            if (!pages->Contains(page))
            {
              pages->Add(page);
            }
          }
        }
      }

      Dictionary<String^, PointF> pageOffset = gcnew Dictionary<String^, PointF>();

      int sqrtPages = (int)Math::Round(Math::Sqrt((double)pages->Count)+0.5);
      int i = 0;
      int j = 0;
      float dX = 0.0F;
      float dY = 0.0F;
      for (int k=0; k<pages->Count; k++)
      {
        String ^ a = pages[k];
        pageOffset.Add("/" + filename + "/" + pages[k] + "/", PointF(dX, dY));

        i++;
        dX += 450.0F;
        if (i > sqrtPages-1)
        {
          i = 0;
          dX = 0.0F;
          j++;
          dY += 300.0F;
        }
      }

      {
      POSITION Pos=m_pUnmanaged->m_Things.GetHeadPosition();
      while (Pos)
        {
          CNSGrfThing * pThing = m_pUnmanaged->m_Things.GetNext(Pos);

          GraphicThing ^ graphicThing = gcnew GraphicThing(Guid(gcnew String(pThing->m_Guid)), gcnew String(pThing->m_Tag));
          String ^ path = "/" + filename + "/" + gcnew String(pThing->m_Page) + "/";

		  ArrayList ^ elements = gcnew ArrayList();
		  elements->Add(gcnew Line(70.0, 95.0, 100.0, 95.0));
		  elements->Add(gcnew Line(100.0, 95.0, 100.0, 100.0));
		  elements->Add(gcnew Line(100.0, 100.0, 70.0, 100.0));
		  elements->Add(gcnew Line(70.0, 100.0, 70.0, 95.0));

		  ArrayList ^ decorations = gcnew ArrayList();
		  decorations->Add(gcnew Line(0.0, 0.0, 100.0, 0.0));
		  decorations->Add(gcnew Line(100.0, 0.0, 100.0, 100.0));
		  decorations->Add(gcnew Line(100.0, 100.0, 0.0, 100.0));
		  decorations->Add(gcnew Line(0.0, 100.0, 0.0, 0.0));

		  ArrayList ^ textArea = gcnew ArrayList();
		  textArea->Add(gcnew Line(70.0, 95.0, 100.0, 95.0));
		  textArea->Add(gcnew Line(100.0, 95.0, 100.0, 100.0));
		  textArea->Add(gcnew Line(100.0, 100.0, 70.0, 100.0));
		  textArea->Add(gcnew Line(70.0, 100.0, 70.0, 95.0));

		  ArrayList ^ animations = gcnew ArrayList();
      //animations->Add(gcnew Animation());

      graphicThing->Populate(filename, gcnew String(pThing->m_Page),
            gcnew String(pThing->m_Guid), 
            RectangleF(pThing->m_Left + pageOffset[path].X, pThing->m_Top + pageOffset[path].Y, pThing->m_Width, pThing->m_Height),
            pThing->m_Rotation, false, false, System::Drawing::Color::Aqua, System::Drawing::Color::Red, gcnew String(pThing->m_Tag),
            elements, decorations, textArea, animations,
            System::Drawing::Drawing2D::FillMode::Alternate);
          graphic->graphicThings->Add(graphicThing->Guid, graphicThing);
        }
      }


      {
      POSITION Pos=m_pUnmanaged->m_Guids.GetHeadPosition();
      while (Pos)
        {
        CNSGuidItem * pGuid = m_pUnmanaged->m_Guids.GetNext(Pos);
        if (!pGuid->m_IsLink)
          {
            CNSMdlNode * pNode = dynamic_cast<CNSMdlNode *>(pGuid); 

            GraphicItem ^ graphicItem = gcnew GraphicItem(Guid(gcnew String(pNode->m_Guid)), gcnew String(pNode->m_Tag));
            String ^ path = "/" + filename + "/" + gcnew String(pNode->m_pGrfs[0]->m_Page) + "/";

            graphicItem->Populate(filename, gcnew String(pNode->m_pGrfs[0]->m_Page),
              gcnew String(pNode->m_Guid), gcnew String(pNode->m_ClassID), 
              RectangleF(pNode->m_pGrfs[0]->m_Left + pageOffset[path].X, pNode->m_pGrfs[0]->m_Top + pageOffset[path].Y, pNode->m_pGrfs[0]->m_Width, pNode->m_pGrfs[0]->m_Height),
              pNode->m_pGrfs[0]->m_Rotation);
            graphic->graphicItems->Add(graphicItem->Guid, graphicItem);
          }
        }
      }

      {
      POSITION Pos=m_pUnmanaged->m_Guids.GetHeadPosition();
      while (Pos)
        {
        CNSGuidItem * pGuid = m_pUnmanaged->m_Guids.GetNext(Pos);
        if (pGuid->m_IsLink)
          {
            CNSMdlLink * pLink = dynamic_cast<CNSMdlLink *>(pGuid); 
            GraphicItem ^originGraphicItem;
            graphic->graphicItems->TryGetValue(Guid(gcnew String(pLink->m_SrcGuid)), originGraphicItem);

            PointF offset = PointF(0.0F, 0.0F);
            pageOffset.TryGetValue(originGraphicItem->Path, offset);

            GraphicLink ^ graphicLink = gcnew GraphicLink(Guid(gcnew String(pLink->m_Guid)), gcnew String(pLink->m_Tag), gcnew String(pLink->m_ClassID), 
            Guid(gcnew String(pLink->m_SrcGuid)), gcnew String(pLink->m_SrcPort), Guid(gcnew String(pLink->m_DstGuid)), gcnew String(pLink->m_DstPort)); //pkh

          CArray <CNSGrfLink::CPt, CNSGrfLink::CPt&> &Pts=pLink->m_pGrf->m_Pts;
          Generic::List<PointF> ^ ControlPoints = gcnew Generic::List<PointF>;

          // Why negative Y ??????
          for (int i=0; i<Pts.GetCount(); i++)
            ControlPoints->Add(PointF(Pts[i].m_X + offset.X, -Pts[i].m_Y + offset.Y));

          graphicLink->Populate(//filename, gcnew String(pLink->m_pGrf->m_Page), 
            gcnew String(pLink->m_Guid), gcnew String(pLink->m_ClassID), 
            gcnew String(pLink->m_SrcGuid), gcnew String(pLink->m_DstGuid),
            gcnew String(pLink->m_SrcPort), gcnew String(pLink->m_DstPort),
            ControlPoints ,
            graphic->graphicItems);

          graphic->graphicLinks->Add(graphicLink->Guid, graphicLink);
          }
      }
      }

      RemotingServices::Marshal(graphic, filename);
      m_Config->ProjectList->Add(filename);
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

//========================================================================

#endif

//========================================================================
