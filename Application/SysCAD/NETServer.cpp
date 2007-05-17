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
	EngineProtocol ^ engineProtocol;

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

  bool ConfirmModelStencil(ModelStencil^ modelstencil)
  {
    // TODO: check whether this stencil is to be included in the project.
    return true;
  }

  bool ValidAnchor(Anchor^ anchor)
  {
    // TODO: Check here if anchor->Tag is to be included. (and possibly in future anchor->Type)
    return true;
  }

  void TrimAnchorPoints(ModelStencil^ modelStencil)
  {
    ArrayList^ anchors = modelStencil->Anchors;
    ArrayList^ validAnchors = gcnew ArrayList();
    for (int i=0; i<anchors->Count; i++)
    {
      Anchor^ anchor = (Anchor^)(anchors[i]);
      if (ValidAnchor(anchor)) 
      {
        validAnchors->Add(anchor);
      }
    }

    modelStencil->Anchors = validAnchors;
  }

  void GetStencils()
  {
    int iStencil = 0;
    LogNote("Srvr", 0, "ModelStencils:");
    array<String^>^dirs = Directory::GetFiles(m_StencilPath, "*.modelstencil");
    for (int i=0; i<dirs->GetLength(0); i++)
    {
      String ^ fullpath = dirs[i];

      ////Create dummy ModelStencil for comparison...
      //{
      //  ModelStencil^ modelStencil = gcnew ModelStencil();
      //  modelStencil->Tag = "";
      //  modelStencil->GroupName = "Control";
      //  ArrayList^ elements = gcnew ArrayList();
      //  SysCAD::Protocol::Arc^ arc = gcnew SysCAD::Protocol::Arc(0, 0, 100, 100, 10, 360);
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

      if (ConfirmModelStencil(modelStencil))
      {
        TrimAnchorPoints(modelStencil);
        m_Config->ModelStencils->Add(Path::GetFileNameWithoutExtension(fullpath), modelStencil);
      }

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

      ////Create dummy GraphicStencil for comparison...
      //{
      //  GraphicStencil^ graphicStencil = gcnew GraphicStencil();
      //  graphicStencil->Tag = "";
      //  ArrayList^ elements = gcnew ArrayList();
      //  SysCAD::Protocol::Arc^ arc = gcnew SysCAD::Protocol::Arc(0, 0, 100, 100, 10, 360);
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

    {
      int iStencil = 0;
      LogNote("Srvr", 0, "ThingStencils:");
      array<String^>^dirs = Directory::GetFiles(m_StencilPath, "*.thingstencil");
      for (int i=0; i<dirs->GetLength(0); i++)
      {
        String ^ fullpath = dirs[i];

        ////Create dummy ThingStencil for comparison...
        //{
        //  ThingStencil^ thingStencil = gcnew ThingStencil();
        //  thingStencil->Tag = "a tank annotation";
        //  thingStencil->Model = "Tank-1";
        //  thingStencil->defaultSize.Width = 20.0;
        //  thingStencil->defaultSize.Height = 20.0;
        //  thingStencil->Xaml = 
        //      "            <!-- Saved from Aurora XAML Designer for WinFX - Mobiform Software Ltd. - Thursday, 4 January 2007 11:45:44 AM -->"
        //      "<Canvas xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" Name=\"Canvas1\" Width=\"132\" Height=\"141\" Background=\"{x:Static Brushes.Transparent}\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
        //      "  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"105\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
        //      "    <Ellipse.Fill>"
        //      "      <LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0.03,1\">"
        //      "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "  <Rectangle Canvas.Left=\"6\" Canvas.Top=\"21\" Width=\"120\" Height=\"100\" Stroke=\"Gray\" StrokeThickness=\"0.5\">"
        //      "    <Rectangle.Fill>"
        //      "      <LinearGradientBrush StartPoint=\"0,1\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Rectangle.Fill>"
        //      "  </Rectangle>"
        //      "  <Rectangle Canvas.Left=\"16\" Canvas.Bottom=\"21\" Width=\"100\" Height=\"100\" Fill=\"Blue\" Stroke=\"Black\">"
        //      "  </Rectangle>"
        //      "  <Rectangle Canvas.Left=\"16\" Canvas.Top=\"31\" Width=\"100\" Height=\"[[[[TAG]], 0, 3, 0, 85, Linear, Integer]]\" Stroke=\"Black\">"
        //      "    <Rectangle.Fill>"
        //      "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"Black\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"Black\" Offset=\"1\" />"
        //      "        <GradientStop Color=\"#FF444444\" Offset=\"0.821339950372208\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Rectangle.Fill>"
        //      "  </Rectangle>"
        //      "  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
        //      "    <Ellipse.Fill>"
        //      "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
        //      "    <Ellipse.Fill>"
        //      "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"129\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">"
        //      "    <Ellipse.Fill>"
        //      "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
        //      "    <Ellipse.Fill>"
        //      "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
        //      "    <Ellipse.Fill>"
        //      "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"2\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">"
        //      "    <Ellipse.Fill>"
        //      "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">"
        //      "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"White\" Offset=\"0.5\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </LinearGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"7\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">"
        //      "    <Ellipse.Fill>"
        //      "      <RadialGradientBrush Opacity=\"1\" Center=\"0.490074441687345,0.5\" RadiusX=\"0.52\" RadiusY=\"0.599255583126551\" GradientOrigin=\"0.16,0.54\">"
        //      "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />"
        //      "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />"
        //      "      </RadialGradientBrush>"
        //      "    </Ellipse.Fill>"
        //      "  </Ellipse>"
        //      "</Canvas>";

        //  SoapFormatter^ sf = gcnew SoapFormatter();
        //  StreamWriter ^ streamWriter = gcnew StreamWriter(fullpath+".new");
        //  Stream^ stream = streamWriter->BaseStream;
        //  sf->Serialize(stream, thingStencil);
        //  stream->Close();
        //}

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

  void MarshalServiceInterface()
  {
    engineProtocol = gcnew EngineProtocol();

    String^ filename = gcnew String(PrjName()); //gs_pPrj->

    RemotingServices::Marshal(engineProtocol, filename);
    m_Config->ProjectList->Add(filename);
    LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
  };


  void Startup()
  {
    LogNote("CNETServerThread", 0, "Startup");

    m_Config = gcnew ConfigData;
    m_StencilPath = gcnew String(BaseCfgFiles());
    m_StencilPath = m_StencilPath + "Stencils\\";

    //m_pUnmanaged = new CNETServerU;

    //m_pUnmanaged->Startup();

    Console::WriteLine("Startup");
    CreateConnects();
    GetStencils();
    MarshalServiceInterface();
    MarshalConfig();

    LoadItems();
  };

  void Shutdown()
  {
    delete m_Config;

    LogNote("CNETServerThread", 0, "Shutdown");
  };


  Dictionary<String^, PointF>^ pageOffset;

  void LoadItems()
  {
    CWaitMsgCursor Wait("9->10");
    String^ filename = gcnew String(PrjName()); //gs_pPrj->

    CDocTemplate & Template = ScdApp()->GraphTemplate();




    StringCollection ^ pages = gcnew StringCollection();
    pageOffset = gcnew Dictionary<String^, PointF>();

    {
      POSITION Pos = Template.GetFirstDocPosition();
      while (Pos)
      {
        CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));

        String^ page = gcnew String(pDoc->GetTitle());


        if (!pages->Contains(page))
        {
          pages->Add(page);
        }

      }

      int sqrtPages = (int)Math::Round(Math::Sqrt((double)pages->Count)+0.5);
      int i = 0;
      int j = 0;
      float dX = 0.0F;
      float dY = 0.0F;
      for (int k=0; k<pages->Count; k++)
      {
        String ^ page = pages[k];
        if (page->Length>4)
          page = page->Substring(0,page->Length-4);//remove .scg
        pageOffset->Add("/" + filename + "/" + page + "/", PointF(dX, dY));

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

    }















    POSITION Pos = Template.GetFirstDocPosition();
    while (Pos)
    {
      CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));

      CGrfTagInfoArray GTIA;

      //if (OpenSymbolExport())
      //  pDoc->GCB.pDrw->SaveSymbols(m_SymbolExportFolder.GetBuffer(), pDoc->m_sDocGUID());
      const int Cnt = pDoc->GetTagListInfo(true, GTIA);
      if (Cnt>0)
      {
        String^ page = gcnew String(pDoc->GetTitle());
        Guid guid;
        if (page->Length>4)
          page = page->Substring(0,page->Length-4);//remove .scg
        POSITION pos = pDoc->GetFirstViewPosition();
        CView* pFirstView = pDoc->GetNextView( pos );
        CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
        for (int i=0; i<Cnt; i++)
        {
          CGrfTagInfo & I = GTIA[i];
          //if (I.m_bHasTag && FindOldTag(I.m_sTag())<0)
          //  continue;

          int TagTyp = -1; //error
          if (I.m_bHasTag)
          {
            FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag());
            if (pNode)
            {
              TagObjClass * pTagObjC = pNode->Class();
              I.m_sClass = pNode->ClassId();
              guid = Guid(gcnew String(pNode->EqpGUID()));


              //CXM_ObjectTag ObjTag(I.m_sTag(), TABOpt_Parms);
              //CXM_ObjectData ObjData;
              //CXM_Route Route;
              //TagObjClass * pTagObjC = NULL;
              //if (pGWnd->XReadTaggedItem(ObjTag, ObjData, Route))
              //  {
              //  I.m_sClass = ObjData.FirstItem()->Class();
              //  pTagObjC = TagObjClass::FindClassId(I.m_sClass());
              //  if (pTagObjC==NULL)
              //    pTagObjC = TagObjClass::FindClassName(I.m_sClass());
              if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 || 
                _stricmp(CtrlLinkGrp, pTagObjC->Group())==0 || 
                _stricmp(ElecLinkGrp, pTagObjC->Group())==0 || 
                _stricmp(AirLinkGrp, pTagObjC->Group())==0))
                TagTyp = 2; //link
              else
                TagTyp = 1; //unit/node
              if (pTagObjC)
              {
                I.m_sDrwGroup=pTagObjC->DrwGroup();
              }

              //Strng GuidTag(I.m_sTag());
              //GuidTag+=".EqpGUID";
              //CXM_ObjectTag GuidObjTag(GuidTag(), TABOpt_Parms);
              //CXM_ObjectData GuidObjData;
              //CXM_Route GuidRoute;
              //if (pGWnd->XReadTaggedItem(GuidObjTag, GuidObjData, GuidRoute))
              //  {
              //  Guid = GuidObjData.FirstItem()->Value()->GetString();
              //  }  
            }
            else
            {
              TagTyp=3;
              I.m_sClass = "";
            }

            if (TagTyp==2)
            {
              CLinePointsArray LPA;
              pDoc->GCB.pDrw->CollectLinkInfo(I, LPA);

              GraphicLink^ graphicLink = gcnew GraphicLink(guid, gcnew String(I.m_sTag()), gcnew String(I.m_sClass()), Guid(gcnew String(pNode->Nd_Rmt(0)->EqpGUID())), gcnew String(pNode->IOArea_Rmt(0).m_pName), Guid(gcnew String(pNode->Nd_Rmt(1)->EqpGUID())), gcnew String(pNode->IOArea_Rmt(1).m_pName));

              graphicLink->ControlPoints = gcnew List<PointF>();
              for (int i=0; i<LPA.GetSize(); i++)
                graphicLink->ControlPoints->Add(PointF(LPA[i].x, -LPA[i].y));

              engineProtocol->graphicLinks->Add(graphicLink->Guid, graphicLink);
            }
            else if (TagTyp==1)// || TagTyp==0)
            {
              Strng tag = I.m_sTag();

              if (tag.Find("FLOWSHEET_")) // _NOT_ a FLOWSHEET_* non-unit -- There must be a better way to find/handle these?
              {
                GraphicItem^ graphicItem = gcnew GraphicItem(guid, gcnew String(I.m_sTag()));
                String ^ path = "/" + filename + "/" + page + "/";

                float left = (float)I.m_LoBnd.m_X + pageOffset[path].X;
                float top = -(float)I.m_HiBnd.m_Y + pageOffset[path].Y;
                float width = (float)I.m_HiBnd.m_X - (float)I.m_LoBnd.m_X;
                float height = (float)I.m_HiBnd.m_Y - (float)I.m_LoBnd.m_Y;

                float rotation  = -(float)I.m_Node.m_Rotation;

                PointF d = pageOffset[path];

                graphicItem->Populate(filename, page,
                  guid.ToString(), gcnew String(I.m_sClass()), 
                  RectangleF(left, top, width, height),
                  rotation);

                engineProtocol->graphicItems->Add(graphicItem->Guid, graphicItem);
              }
              else
              {
                // This creates a dummy 'tank-1' thing annotation.
                //DoneOne = true;
                //CNSGrfThing * pGrf=new CNSGrfThing(I.m_sTag(), Guid(), Page(), I);
                //m_Things.AddTail(pGrf);
              }
            }
          }
          //if (TagTyp==3)  
          //  {
          //  I.m_sTag.Set("$%i$", ++InsertCnt);
          //  I.m_sClass="Insert";
          //  I.m_sDrwGroup="Inserts";

          //  DoneOne=true;
          //  WriteInsert(m_sGroup, I, m_Origin, Page(), pDoc->m_sDocGUID());
          //  ExportSymbol(I, m_Origin, pDoc->m_sDocGUID());
          //  }
        }
        //if (m_bWithGroups && DoneOne)
        //  WriteGroupInfo(m_sGroup, m_sGroupDesc, m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
      }

      //CloseSymbolExport();


      //  RemotingServices::Marshal(graphic, filename);
      //  m_Config->projectList->Add(filename);
      //  LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
    }

    {
      for each (GraphicLink^ graphicLink in engineProtocol->graphicLinks->Values)
      {
        PointF offset = PointF::Empty;
        {
          String^ path = "";

          Guid origin = graphicLink->Origin;
          Guid destination = graphicLink->Destination;

          if (origin != Guid::Empty)
            path = engineProtocol->graphicItems[origin]->Path;
          else if (destination != Guid::Empty)
            path = engineProtocol->graphicItems[destination]->Path;

          if (path->Length>0)
            offset = pageOffset[path];
        }

        List<PointF>^ oldControlPoints = graphicLink->ControlPoints;
        List<PointF>^ newControlPoints = gcnew List<PointF>();

        for each (PointF point in oldControlPoints)
        {
          point.X += offset.X;
          point.Y += offset.Y;
          newControlPoints->Add(point);
        }
        graphicLink->ControlPoints = newControlPoints;
      }
    }

  };










char wide_to_narrow(wchar_t w)
{
  return char(w);
}




  void SaveItems()
  {
    //Assmue nothign is deleted, created or re-pathed!
    //Also (consequently) assuming that pageOffset is correct and up to date.
      //It should have been filled properly at project load...

    CWaitMsgCursor Wait("10->9");
    String^ filename = gcnew String(PrjName()); //gs_pPrj->

    CDocTemplate & Template = ScdApp()->GraphTemplate();

    POSITION Pos = Template.GetFirstDocPosition();
    while (Pos)
    {
      CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));

      CGrfTagInfoArray GTIA;

      //if (OpenSymbolExport())
      //  pDoc->GCB.pDrw->SaveSymbols(m_SymbolExportFolder.GetBuffer(), pDoc->m_sDocGUID());
      const int Cnt = pDoc->GetTagListInfo(true, GTIA);
      if (Cnt>0)
      {
        String^ page = gcnew String(pDoc->GetTitle());
        Guid guid;
        if (page->Length>4)
          page = page->Substring(0,page->Length-4);//remove .scg
        POSITION pos = pDoc->GetFirstViewPosition();
        CView* pFirstView = pDoc->GetNextView( pos );
        CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
        for (int i=0; i<Cnt; i++)
        {
          CGrfTagInfo & I = GTIA[i];
          //if (I.m_bHasTag && FindOldTag(I.m_sTag())<0)
          //  continue;

          int TagTyp = -1; //error
          if (I.m_bHasTag)
          {
            FlwNode * pNode = gs_pSfeSrvr->FE_FindNode(I.m_sTag());
            if (pNode)
            {
              TagObjClass * pTagObjC = pNode->Class();
              I.m_sClass = pNode->ClassId();
              guid = Guid(gcnew String(pNode->EqpGUID()));


              //CXM_ObjectTag ObjTag(I.m_sTag(), TABOpt_Parms);
              //CXM_ObjectData ObjData;
              //CXM_Route Route;
              //TagObjClass * pTagObjC = NULL;
              //if (pGWnd->XReadTaggedItem(ObjTag, ObjData, Route))
              //  {
              //  I.m_sClass = ObjData.FirstItem()->Class();
              //  pTagObjC = TagObjClass::FindClassId(I.m_sClass());
              //  if (pTagObjC==NULL)
              //    pTagObjC = TagObjClass::FindClassName(I.m_sClass());
              if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC->Group())==0 || 
                _stricmp(CtrlLinkGrp, pTagObjC->Group())==0 || 
                _stricmp(ElecLinkGrp, pTagObjC->Group())==0 || 
                _stricmp(AirLinkGrp, pTagObjC->Group())==0))
                TagTyp = 2; //link
              else
                TagTyp = 1; //unit/node
              if (pTagObjC)
              {
                I.m_sDrwGroup=pTagObjC->DrwGroup();
              }

              //Strng GuidTag(I.m_sTag());
              //GuidTag+=".EqpGUID";
              //CXM_ObjectTag GuidObjTag(GuidTag(), TABOpt_Parms);
              //CXM_ObjectData GuidObjData;
              //CXM_Route GuidRoute;
              //if (pGWnd->XReadTaggedItem(GuidObjTag, GuidObjData, GuidRoute))
              //  {
              //  Guid = GuidObjData.FirstItem()->Value()->GetString();
              //  }  
            }
            else
            {
              TagTyp=3;
              I.m_sClass = "";
            }

            if (TagTyp==2)
            {
              CLinePointsArray LPA;
              pDoc->GCB.pDrw->CollectLinkInfo(I, LPA);

              //GraphicLink^ graphicLink = gcnew GraphicLink(guid, gcnew String(I.m_sTag()), gcnew String(I.m_sClass()), Guid(gcnew String(pNode->Nd_Rmt(0)->EqpGUID())), gcnew String(pNode->IOArea_Rmt(0).m_pName), Guid(gcnew String(pNode->Nd_Rmt(1)->EqpGUID())), gcnew String(pNode->IOArea_Rmt(1).m_pName));

              //graphicLink->ControlPoints = gcnew List<PointF>();
              //for (int i=0; i<LPA.GetSize(); i++)
              //  graphicLink->ControlPoints->Add(PointF(LPA[i].x, -LPA[i].y));

              //engineProtocol->graphicLinks->Add(graphicLink->Guid, graphicLink);
            }
            else if (TagTyp==1)// || TagTyp==0)
            {
              Strng tag = I.m_sTag();

              if (tag.Find("FLOWSHEET_")) // _NOT_ a FLOWSHEET_* non-unit -- There must be a better way to find/handle these?
              {
                GraphicItem^ graphicItem = engineProtocol->graphicItems[guid];

                // This is the code for converting the String->char*, 
                // and then the dummy call to modify the item.  *********

                // The structure of the above has been changed many times, 
                // and possibly (probably?) no longer produces the correct pDoc...
                // One solution (temporary) if that is difficult is to process _all_
                // items once for each pDoc, and have the below function drop out 
                // if tagc isn't found in the pDoc.

                int length = graphicItem->Tag->Length;
                wchar_t * tagwc = (wchar_t*)(void*)Marshal::StringToHGlobalUni(graphicItem->Tag);

                char * tagc = (char *)malloc(length+1);
                tagc[length] = 0;
                for (int i=0; i<length; i++)
                  tagc[i] = (char)tagwc[i];

                pDoc->GCB.DoModify(tagc, 
                                    graphicItem->Angle,
                                    graphicItem->BoundingRect.X, 
                                    graphicItem->BoundingRect.Y, 
                                    graphicItem->BoundingRect.Width, 
                                    graphicItem->BoundingRect.Height, 
                                    graphicItem->FillColor.ToArgb(),
                                    graphicItem->MirrorX,
                                    graphicItem->MirrorY);


                //pDoc->GCB.pDrw->TranslateEntity(Like, pDsp->StartPt.World, pDsp->CurrentPt.World);
                //gs_pCmd->ExtendCmdLine(";"); //SelectionList will be clear or invalid, cannot continue repositioning !!!
                //pDrw->EntityInvalidate(Like, NULL);
                //pDsp->Draw(Like, -1);basdffruyygiuh;oi7

                //I.m_LoBnd.m_X = graphicItem->BoundingRect.Left - pageOffset[graphicItem->Path].X;
                //I.m_HiBnd.m_Y = -(graphicItem->BoundingRect.Top - pageOffset[graphicItem->Path].Y);
                //I.m_HiBnd.m_X = (float)I.m_LoBnd.m_X + graphicItem->BoundingRect.Width;
                //I.m_LoBnd.m_Y = (float)I.m_HiBnd.m_Y - graphicItem->BoundingRect.Height;

                //I.m_Node.m_Rotation = -graphicItem->Angle;
              }
              else
              {
                // This creates a dummy 'tank-1' thing annotation.
                //DoneOne = true;
                //CNSGrfThing * pGrf=new CNSGrfThing(I.m_sTag(), Guid(), Page(), I);
                //m_Things.AddTail(pGrf);
              }
            }
          }
          //if (TagTyp==3)  
          //  {
          //  I.m_sTag.Set("$%i$", ++InsertCnt);
          //  I.m_sClass="Insert";
          //  I.m_sDrwGroup="Inserts";

          //  DoneOne=true;
          //  WriteInsert(m_sGroup, I, m_Origin, Page(), pDoc->m_sDocGUID());
          //  ExportSymbol(I, m_Origin, pDoc->m_sDocGUID());
          //  }
        }
        //if (m_bWithGroups && DoneOne)
        //  WriteGroupInfo(m_sGroup, m_sGroupDesc, m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
      }

      //CloseSymbolExport();


      //  RemotingServices::Marshal(graphic, filename);
      //  m_Config->projectList->Add(filename);
      //  LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
    }
  }













  void Load()
  {
    engineProtocol->Load();
  };

  void Save()
  {
    engineProtocol->Save();
  };
  
protected:
  SysCAD::Protocol::ConfigData ^ m_Config;
  String ^ m_StencilPath;
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
