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

                    //Create dummy ModelStencil for comparison...
                    {
                      ModelStencil^ modelStencil = gcnew ModelStencil();
                      modelStencil->Tag = "";
                      modelStencil->GroupName = "Control";
                      ArrayList^ elements = gcnew ArrayList();
                      SysCAD::Protocol::Arc^ arc = gcnew SysCAD::Protocol::Arc(0, 0, 100, 100, 10, 360);
                      elements->Add(arc);
                      modelStencil->Elements = elements;

                      modelStencil->Decorations = gcnew ArrayList();
                      modelStencil->Anchors = gcnew ArrayList();
                      modelStencil->FillMode = System::Drawing::Drawing2D::FillMode::Alternate;

                      SoapFormatter^ sf = gcnew SoapFormatter();
                      StreamWriter ^ streamWriter = gcnew StreamWriter(fullpath+".new");
                      Stream^ stream = streamWriter->BaseStream;
                      sf->Serialize(stream, modelStencil);
                      stream->Close();
                    }

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

                    //Create dummy GraphicStencil for comparison...
                    {
                      GraphicStencil^ graphicStencil = gcnew GraphicStencil();
                      graphicStencil->Tag = "";
                      ArrayList^ elements = gcnew ArrayList();
                      SysCAD::Protocol::Arc^ arc = gcnew SysCAD::Protocol::Arc(0, 0, 100, 100, 10, 360);
                      elements->Add(arc);
                      graphicStencil->Elements = elements;

                      graphicStencil->Decorations = gcnew ArrayList();

                      SoapFormatter^ sf = gcnew SoapFormatter();
                      StreamWriter ^ streamWriter = gcnew StreamWriter(fullpath+".new");
                      Stream^ stream = streamWriter->BaseStream;
                      sf->Serialize(stream, graphicStencil);
                      stream->Close();
                    }

        SoapFormatter ^ sf = gcnew SoapFormatter;
        Stream ^ stream = (gcnew StreamReader(fullpath))->BaseStream;
        GraphicStencil ^ graphicStencil = (GraphicStencil^)sf->Deserialize(stream);
        stream->Close();


        graphicStencil->Tag = Path::GetFileNameWithoutExtension(fullpath);
        m_Config->GraphicStencils->Add(Path::GetFileNameWithoutExtension(fullpath), graphicStencil);
        //Console::WriteLine("  {0}] {1}", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        }

      {
      int iStencil = 0;
      LogNote("Srvr", 0, "ThingStencils:");
      array<String^>^dirs = Directory::GetFiles(m_StencilPath, "*.thingstencil");
      for (int i=0; i<dirs->GetLength(0); i++)
        {
        String ^ fullpath = dirs[i];

                    //Create dummy ThingStencil for comparison...
                    {
                      ThingStencil^ thingStencil = gcnew ThingStencil();
                      thingStencil->Tag = "a tank annotation";
                      thingStencil->Model = "Tank-1";
                      thingStencil->defaultSize.Width = 20.0;
                      thingStencil->defaultSize.Height = 20.0;
                      thingStencil->Xaml = 
                          "            <!-- Saved from Aurora XAML Designer for WinFX - Mobiform Software Ltd. - Thursday, 4 January 2007 11:45:44 AM -->"
                          "<Canvas xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" Name=\"Canvas1\" Width=\"132\" Height=\"141\" Background=\"{x:Static Brushes.Transparent}\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
                          "  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"105\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
                          "    <Ellipse.Fill>"
                          "      <LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0.03,1\">"
                          "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "  <Rectangle Canvas.Left=\"6\" Canvas.Top=\"21\" Width=\"120\" Height=\"100\" Stroke=\"Gray\" StrokeThickness=\"0.5\">"
                          "    <Rectangle.Fill>"
                          "      <LinearGradientBrush StartPoint=\"0,1\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Rectangle.Fill>"
                          "  </Rectangle>"
                          "  <Rectangle Canvas.Left=\"16\" Canvas.Bottom=\"21\" Width=\"100\" Height=\"100\" Fill=\"Blue\" Stroke=\"Black\">"
                          "  </Rectangle>"
                          "  <Rectangle Canvas.Left=\"16\" Canvas.Top=\"31\" Width=\"100\" Height=\"[[[[TAG]], 0, 3, 0, 85, Linear, Integer]]\" Stroke=\"Black\">"
                          "    <Rectangle.Fill>"
                          "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"Black\" Offset=\"0\" />"
                          "        <GradientStop Color=\"Black\" Offset=\"1\" />"
                          "        <GradientStop Color=\"#FF444444\" Offset=\"0.821339950372208\" />"
                          "      </LinearGradientBrush>"
                          "    </Rectangle.Fill>"
                          "  </Rectangle>"
                          "  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
                          "    <Ellipse.Fill>"
                          "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
                          "    <Ellipse.Fill>"
                          "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"129\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">"
                          "    <Ellipse.Fill>"
                          "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
                          "    <Ellipse.Fill>"
                          "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
                          "    <Ellipse.Fill>"
                          "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"2\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">"
                          "    <Ellipse.Fill>"
                          "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
                          "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </LinearGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"7\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
                          "    <Ellipse.Fill>"
                          "      <RadialGradientBrush Opacity=\"1\" Center=\"0.490074441687345,0.5\" RadiusX=\"0.52\" RadiusY=\"0.599255583126551\" GradientOrigin=\"0.16,0.54\">"
                          "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
                          "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
                          "      </RadialGradientBrush>"
                          "    </Ellipse.Fill>"
                          "  </Ellipse>"
                          "</Canvas>";

                      SoapFormatter^ sf = gcnew SoapFormatter();
                      StreamWriter ^ streamWriter = gcnew StreamWriter(fullpath+".new");
                      Stream^ stream = streamWriter->BaseStream;
                      sf->Serialize(stream, thingStencil);
                      stream->Close();
                    }

        SoapFormatter ^ sf = gcnew SoapFormatter;
        StreamReader ^ streamRdr = gcnew StreamReader(fullpath);
        Stream ^ stream = streamRdr->BaseStream;
        ThingStencil ^ thingStencil = (ThingStencil^)sf->Deserialize(stream);
        thingStencil->Tag = Path::GetFileNameWithoutExtension(fullpath);
        m_Config->ThingStencils->Add(Path::GetFileNameWithoutExtension(fullpath), thingStencil);
        stream->Close();
        //Console::WriteLine("  {0}] {1}", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path::GetFileNameWithoutExtension(fullpath));
        }
      }
    
    }

    void MarshalConfig()
      {
      RemotingServices::Marshal(m_Config, "Global");
      }

    bool ChangeState(ServiceProtocol^ serviceProtocol, Int64 requestID, BaseProtocol::RunStates runState)
      {
      if (true) // Decide whether to allow runstate change
        { // We're going to do it.
        // Change the runstate.

        // Raise event(s).
        serviceProtocol->DoStateChanged(requestID, runState);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    void GetPropertyValues(ServiceProtocol^ serviceProtocol, Int64 requestID, ArrayList^% propertyList)
      {
        // Return modified ArrayList with tag details included.
      }

    void GetSubTags(ServiceProtocol^ serviceProtocol, Int64 requestID, String^ propertyPath, ArrayList^% propertyList)
      {
        Random^ random = gcnew Random();

        // Return ArrayList with tag details included.
        for (int i=0; i<10; i++)
        {
          StringBuilder^ builder = gcnew StringBuilder();
          System::Char ch;
          int n = random->Next(5, 10);
          for (int i = 0; i < n; i++)
          {
            ch = Convert::ToChar(random->Next(33, 126));
            builder->Append(ch);
          }
          switch (random->Next(0, 3))
          {
            case 0:
              propertyList->Add(gcnew ModelProperty(builder->ToString(), gcnew Int64(random->Next())));
              break;
            case 1:
              propertyList->Add(gcnew ModelProperty(builder->ToString(), gcnew Double(random->NextDouble())));
              break;
            case 2:
              propertyList->Add(gcnew ModelProperty(builder->ToString(), builder->ToString()));
              break;
            case 3:
              propertyList->Add(gcnew ModelProperty(builder->ToString(), Color::FromArgb(random->Next())));
              break;
          }
        }
      }


    bool CreateItem(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to create an item.
        { // We're going to do it.
        // Create the item.

        // Raise event(s).
        serviceProtocol->DoItemCreated(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool ModifyItem(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to modify an item.
        { // We're going to do it.
        // Modify the item.

        // Raise event(s).
       serviceProtocol->DoItemModified(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool DeleteItem(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid)
      {
      if (true) // Decide whether to delete an item.
        { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        serviceProtocol->DoItemDeleted(requestID, guid);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool CreateLink(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid, String^ tag, String^ classID, Guid origin, Guid destination, String^ originPort, String^ destinationPort, Generic::List<PointF>^ controlPoints)
      {
      if (true) // Decide whether to create an link.
        { // We're going to do it.
        // Create the item.

        // Raise event(s).
        serviceProtocol->DoLinkCreated(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool ModifyLink(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid, String^ tag, String^ classID, Guid origin, Guid destination, String^ originPort, String^ destinationPort, Generic::List<PointF>^ controlPoints)
      {
      if (true) // Decide whether to modify an link.
        { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        serviceProtocol->DoLinkModified(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool DeleteLink(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid)
      {
      if (true) // Decide whether to delete an link.
        { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        serviceProtocol->DoLinkDeleted(requestID, guid);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool CreateThing(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid, String^ tag, String^ path, RectangleF boundingRect, String^ xaml, Single angle, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to create an Thing.
        { // We're going to do it.
        // Create the Thing.

        // Raise event(s).
        serviceProtocol->DoThingCreated(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool ModifyThing(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid, String^ tag, String^ path, RectangleF boundingRect, String^ xaml, Single angle, bool mirrorX, bool mirrorY)
      {
      if (true) // Decide whether to modify an Thing.
        { // We're going to do it.
        // Modify the Thing.

        // Raise event(s).
       serviceProtocol->DoThingModified(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    bool DeleteThing(ServiceProtocol^ serviceProtocol, Int64 requestID, Guid guid)
      {
      if (true) // Decide whether to delete an Thing.
        { // We're going to do it.
        // Delete the Thing.

        // Raise event(s).
        serviceProtocol->DoThingDeleted(requestID, guid);

        return true;
        }
      else
        { // We're not going to do it.
        return false;
        }
      }

    PortStatus PortCheck(ServiceProtocol^ serviceProtocol, Guid guid, Anchor^ anchor)
      {
      //		CNSGuidItem * pGuid = new CNSGuidItem();
      //		pGuid->m_Guid = guid;
      //		m_pUnmanaged->m_Guids.AddTail(null);
      //		CNSGuidItem * pGuid = m_pUnmanaged->m_Guids.Find(guid);
      //		CNSMdlLink * pLink = dynamic_cast<CNSMdlLink *>(guid);
      return PortStatus::Available;
      }

    ArrayList^ PropertyListCheck(ServiceProtocol^ serviceProtocol, Guid guid, String^ tag, String^ path)
      {
        char* dest = new char[tag->Length+1];
        strcpy(dest, static_cast<LPCTSTR>(const_cast<void*>(static_cast<const void*>(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(tag)))));
        ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)dest);

        ArrayList^ list = gcnew ArrayList();
        // Generate list of properties at this level in properties tree.
        return list;
      }

    void MarshalServiceInterface()
    {
      ServiceProtocol::ChangeStateHandler^ changeState = gcnew ServiceProtocol::ChangeStateHandler(this, &CNETServerThread::ChangeState);

      ServiceProtocol::GetPropertyValuesHandler^ getPropertyValues = gcnew ServiceProtocol::GetPropertyValuesHandler(this, &CNETServerThread::GetPropertyValues);
      ServiceProtocol::GetSubTagsHandler^ getSubTags = gcnew ServiceProtocol::GetSubTagsHandler(this, &CNETServerThread::GetSubTags);

      ServiceProtocol::CreateItemHandler^ createItem = gcnew ServiceProtocol::CreateItemHandler(this, &CNETServerThread::CreateItem);
      ServiceProtocol::ModifyItemHandler^ modifyItem = gcnew ServiceProtocol::ModifyItemHandler(this, &CNETServerThread::ModifyItem);
      ServiceProtocol::DeleteItemHandler^ deleteItem = gcnew ServiceProtocol::DeleteItemHandler(this, &CNETServerThread::DeleteItem);

      ServiceProtocol::CreateLinkHandler^ createLink = gcnew ServiceProtocol::CreateLinkHandler(this, &CNETServerThread::CreateLink);
      ServiceProtocol::ModifyLinkHandler^ modifyLink = gcnew ServiceProtocol::ModifyLinkHandler(this, &CNETServerThread::ModifyLink);
      ServiceProtocol::DeleteLinkHandler^ deleteLink = gcnew ServiceProtocol::DeleteLinkHandler(this, &CNETServerThread::DeleteLink);

      ServiceProtocol::CreateThingHandler^ createThing = gcnew ServiceProtocol::CreateThingHandler(this, &CNETServerThread::CreateThing);
      ServiceProtocol::ModifyThingHandler^ modifyThing = gcnew ServiceProtocol::ModifyThingHandler(this, &CNETServerThread::ModifyThing);
      ServiceProtocol::DeleteThingHandler^ deleteThing = gcnew ServiceProtocol::DeleteThingHandler(this, &CNETServerThread::DeleteThing);

      ServiceProtocol::PortCheckHandler^ portCheck = gcnew ServiceProtocol::PortCheckHandler(this, &CNETServerThread::PortCheck);

      ServiceProtocol::PropertyListHandler^ propertyListCheck = gcnew ServiceProtocol::PropertyListHandler(this, &CNETServerThread::PropertyListCheck);


      ServiceProtocol ^ serviceProtocol = gcnew ServiceProtocol(changeState, getPropertyValues, getSubTags, createItem, modifyItem, deleteItem, createLink, modifyLink, deleteLink, createThing, modifyThing, deleteThing, portCheck, propertyListCheck);

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

          //"<Image xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" Stretch=\"None\" HorizontalAlignment=\"Left\">\n<Image.Source>\n<DrawingImage PresentationOptions:Freeze=\"True\">\n<DrawingImage.Drawing>\n<GeometryDrawing>\n<GeometryDrawing.Geometry>\n<!-- Create a composite shape. -->\n<GeometryGroup>\n<EllipseGeometry Center=\"50,50\" RadiusX=\"45\" RadiusY=\"20\" />\n<EllipseGeometry Center=\"50,50\" RadiusX=\"20\" RadiusY=\"45\" />\n</GeometryGroup>\n</GeometryDrawing.Geometry>\n<GeometryDrawing.Brush>\n<!-- Paint the drawing with a gradient. -->\n<LinearGradientBrush>\n<GradientStop Offset=\"0.0\" Color=\"Blue\" />\n<GradientStop Offset=\"1.0\" Color=\"#CCCCFF\" />\n</LinearGradientBrush>\n</GeometryDrawing.Brush>\n<GeometryDrawing.Pen>\n<!-- Outline the drawing with a solid color. -->\n<Pen Thickness=\"10\" Brush=\"Black\" />\n</GeometryDrawing.Pen>\n</GeometryDrawing>\n</DrawingImage.Drawing>\n</DrawingImage>\n</Image.Source>\n</Image>\n";
          //"<Image xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\">\n<Image.Source>\n<DrawingImage>\n<DrawingImage.Drawing>\n<GeometryDrawing>\n<GeometryDrawing.Geometry>\n<!-- Create a composite shape. -->\n<GeometryGroup>\n<EllipseGeometry Center=\"50,50\" RadiusX=\"[[Tag, 0, 3, 1, 40, Linear, Integer]]\" RadiusY=\"[[Tag, 0, 3, 1, 40, Linear, Integer]]\" />\n<EllipseGeometry Center=\"50,50\" RadiusX=\"40\" RadiusY=\"20\" />\n</GeometryGroup>\n</GeometryDrawing.Geometry>\n<GeometryDrawing.Brush>\n<!-- Paint the drawing with a gradient. -->\n<LinearGradientBrush>\n<GradientStop Offset=\"0.0\" Color=\"[[Tag, 0, 3, #100000, #ffffff, Linear, Hex]]\" />\n<GradientStop Offset=\"1.0\" Color=\"#CCCCFF\" />\n</LinearGradientBrush>\n</GeometryDrawing.Brush>\n<GeometryDrawing.Pen>\n<!-- Outline the drawing with a solid color. -->\n<Pen Thickness=\"1\" Brush=\"Black\" />\n</GeometryDrawing.Pen>\n</GeometryDrawing>\n</DrawingImage.Drawing>\n</DrawingImage>\n</Image.Source>\n</Image>\n";

          //StreamReader^ reader = gcnew StreamReader("c:\\tank.xaml");
          //String^ xaml = reader->ReadToEnd();

          String^ xaml = 
"            <!-- Saved from Aurora XAML Designer for WinFX - Mobiform Software Ltd. - Thursday, 4 January 2007 11:45:44 AM -->"
"<Canvas xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" Name=\"Canvas1\" Width=\"132\" Height=\"141\" Background=\"{x:Static Brushes.Transparent}\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
"  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"105\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
"    <Ellipse.Fill>"
"      <LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0.03,1\">"
"        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"  <Rectangle Canvas.Left=\"6\" Canvas.Top=\"21\" Width=\"120\" Height=\"100\" Stroke=\"Gray\" StrokeThickness=\"0.5\">"
"    <Rectangle.Fill>"
"      <LinearGradientBrush StartPoint=\"0,1\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Rectangle.Fill>"
"  </Rectangle>"
"  <Rectangle Canvas.Left=\"16\" Canvas.Bottom=\"21\" Width=\"100\" Height=\"100\" Fill=\"Blue\" Stroke=\"Black\">"
"  </Rectangle>"
"  <Rectangle Canvas.Left=\"16\" Canvas.Top=\"31\" Width=\"100\" Height=\"[[Tag, 0, 3, 0, 85, Linear, Integer]]\" Stroke=\"Black\">"
"    <Rectangle.Fill>"
"      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"Black\" Offset=\"0\" />"
"        <GradientStop Color=\"Black\" Offset=\"1\" />"
"        <GradientStop Color=\"#FF444444\" Offset=\"0.821339950372208\" />"
"      </LinearGradientBrush>"
"    </Rectangle.Fill>"
"  </Rectangle>"
"  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
"    <Ellipse.Fill>"
"      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
"        <GradientStop Color=\"White\" Offset=\"0.5\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
"    <Ellipse.Fill>"
"      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
"        <GradientStop Color=\"White\" Offset=\"0.5\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"129\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">"
"    <Ellipse.Fill>"
"      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
"        <GradientStop Color=\"White\" Offset=\"0.5\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
"    <Ellipse.Fill>"
"      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
"        <GradientStop Color=\"White\" Offset=\"0.5\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
"    <Ellipse.Fill>"
"      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
"        <GradientStop Color=\"White\" Offset=\"0.5\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"2\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">"
"    <Ellipse.Fill>"
"      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
"        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
"        <GradientStop Color=\"White\" Offset=\"0.5\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </LinearGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"7\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
"    <Ellipse.Fill>"
"      <RadialGradientBrush Opacity=\"1\" Center=\"0.490074441687345,0.5\" RadiusX=\"0.52\" RadiusY=\"0.599255583126551\" GradientOrigin=\"0.16,0.54\">"
"        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
"        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
"      </RadialGradientBrush>"
"    </Ellipse.Fill>"
"  </Ellipse>"
"</Canvas>";

      graphicThing->Populate(filename, gcnew String(pThing->m_Page),
            gcnew String(pThing->m_Guid), 
            RectangleF(pThing->m_Left + pageOffset[path].X, pThing->m_Top + pageOffset[path].Y, pThing->m_Width, pThing->m_Height),
            xaml, pThing->m_Rotation, false, false);
            serviceProtocol->graphicThings->Add(graphicThing->Guid, graphicThing);
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
            serviceProtocol->graphicItems->Add(graphicItem->Guid, graphicItem);
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
            serviceProtocol->graphicItems->TryGetValue(Guid(gcnew String(pLink->m_SrcGuid)), originGraphicItem);

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
            serviceProtocol->graphicItems);

          serviceProtocol->graphicLinks->Add(graphicLink->Guid, graphicLink);
          }
      }
      }

      RemotingServices::Marshal(serviceProtocol, filename);
      m_Config->ProjectList->Add(filename);
      LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
      };


    void Startup()
      {
      LogNote("CNETServerThread", 0, "Startup");

      m_Config = gcnew ConfigData;
      m_StencilPath = gcnew String(BaseCfgFiles());
      m_StencilPath = m_StencilPath + "Stencils\\";

      m_pUnmanaged = new CNETServerU;

      m_pUnmanaged->Startup();

      Console::WriteLine("Startup");
      CreateConnects();
      GetStencils();
      MarshalServiceInterface();
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
    SysCAD::Protocol::ConfigData ^ m_Config;
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
