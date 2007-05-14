using System;
using System.Collections.Generic;
using System.Text;

using System.Runtime.Remoting.Channels;
using System.Runtime.Serialization.Formatters;
using System.Collections;
using System.Runtime.Remoting.Channels.Ipc;
using SysCAD.Protocol;
using System.Drawing;
using System.Runtime.Remoting;
using System.IO;
using System.Runtime.Serialization.Formatters.Soap;

namespace NetServer
{
  class NetServer
  {
    ConfigData m_Config;
    String m_StencilPath;
    ServiceProtocol serviceProtocol;


  void CreateConnects()
  {
    BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
    serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

    BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

    Hashtable ipcProps = new Hashtable();
    ipcProps["portName"] = "SysCAD.Service";
    //ipcProps["typeFilterLevel"] = TypeFilterLevel.Full;
    IpcChannel ipcChannel = new IpcChannel(ipcProps, clientProv, serverProv);
    ChannelServices.RegisterChannel(ipcChannel, false);
  }

  bool ConfirmModelStencil(ModelStencil modelstencil)
  {
    // TODO: check whether this stencil is to be included in the project.
    return true;
  }

  bool ValidAnchor(Anchor anchor)
  {
    // TODO: Check here if anchor.Tag is to be included. (and possibly in future anchor.Type)
    return true;
  }

  void TrimAnchorPoints(ModelStencil modelStencil)
  {
    ArrayList anchors = modelStencil.Anchors;
    ArrayList validAnchors = new ArrayList();
    for (int i=0; i<anchors.Count; i++)
    {
      Anchor anchor = (Anchor)(anchors[i]);
      if (ValidAnchor(anchor)) 
      {
        validAnchors.Add(anchor);
      }
    }

    modelStencil.Anchors = validAnchors;
  }

  void GetStencils()
  {
    {
      int iStencil = 0;
      LogNote("Srvr", 0, "ModelStencils:");
      String[] dirs = Directory.GetFiles(m_StencilPath, "*.modelstencil");
      for (int i = 0; i < dirs.GetLength(0); i++)
      {
        String fullpath = dirs[i];

        ////Create dummy ModelStencil for comparison...
        //{
        //  ModelStencil modelStencil = new ModelStencil();
        //  modelStencil.Tag = "";
        //  modelStencil.GroupName = "Control";
        //  ArrayList elements = new ArrayList();
        //  SysCAD.Protocol.Arc arc = new SysCAD.Protocol.Arc(0, 0, 100, 100, 10, 360);
        //  elements.Add(arc);
        //  modelStencil.Elements = elements;

        //  modelStencil.Decorations = new ArrayList();
        //  modelStencil.Anchors = new ArrayList();
        //  modelStencil.FillMode = System.Drawing.Drawing2D.FillMode.Alternate;

        //  SoapFormatter sf = new SoapFormatter();
        //  StreamWriter streamWriter = new StreamWriter(fullpath+".new");
        //  Stream stream = streamWriter.BaseStream;
        //  sf.Serialize(stream, modelStencil);
        //  stream.Close();
        //}

        SoapFormatter sf = new SoapFormatter();
        StreamReader streamRdr = new StreamReader(fullpath);
        Stream stream = streamRdr.BaseStream;
        ModelStencil modelStencil = (ModelStencil)sf.Deserialize(stream);
        modelStencil.Tag = Path.GetFileNameWithoutExtension(fullpath);

        if (ConfirmModelStencil(modelStencil))
        {
          TrimAnchorPoints(modelStencil);
          m_Config.ModelStencils.Add(Path.GetFileNameWithoutExtension(fullpath), modelStencil);
        }

        stream.Close();
        //Console.WriteLine("  {0}] {1}", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
      }

      Console.WriteLine("\n");
    }

    {
      int iStencil = 0;
      LogNote("Srvr", 0, "GraphicStencils:");
      String[] dirs = Directory.GetFiles(m_StencilPath, "*.graphicstencil");
      for (int i = 0; i < dirs.GetLength(0); i++)
      {
        String fullpath = dirs[i];

        ////Create dummy GraphicStencil for comparison...
        //{
        //  GraphicStencil graphicStencil = new GraphicStencil();
        //  graphicStencil.Tag = "";
        //  ArrayList elements = new ArrayList();
        //  SysCAD.Protocol.Arc arc = new SysCAD.Protocol.Arc(0, 0, 100, 100, 10, 360);
        //  elements.Add(arc);
        //  graphicStencil.Elements = elements;

        //  graphicStencil.Decorations = new ArrayList();

        //  SoapFormatter sf = new SoapFormatter();
        //  StreamWriter streamWriter = new StreamWriter(fullpath+".new");
        //  Stream stream = streamWriter.BaseStream;
        //  sf.Serialize(stream, graphicStencil);
        //  stream.Close();
        //}

        SoapFormatter sf = new SoapFormatter();
        Stream stream = (new StreamReader(fullpath)).BaseStream;
        GraphicStencil graphicStencil = (GraphicStencil)sf.Deserialize(stream);
        stream.Close();


        graphicStencil.Tag = Path.GetFileNameWithoutExtension(fullpath);
        m_Config.GraphicStencils.Add(Path.GetFileNameWithoutExtension(fullpath), graphicStencil);
        //Console.WriteLine("  {0}] {1}", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
      }
    }

    {
      int iStencil = 0;
      LogNote("Srvr", 0, "ThingStencils:");
      String[] dirs = Directory.GetFiles(m_StencilPath, "*.thingstencil");
      for (int i = 0; i < dirs.GetLength(0); i++)
      {
        String fullpath = dirs[i];

        ////Create dummy ThingStencil for comparison...
        //{
        //  ThingStencil thingStencil = new ThingStencil();
        //  thingStencil.Tag = "a tank annotation";
        //  thingStencil.Model = "Tank-1";
        //  thingStencil.defaultSize.Width = 20.0;
        //  thingStencil.defaultSize.Height = 20.0;
        //  thingStencil.Xaml = 
        //      "            <!-- Saved from Aurora XAML Designer for WinFX - Mobiform Software Ltd. - Thursday, 4 January 2007 11:45:44 AM -."
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

        //  SoapFormatter sf = new SoapFormatter();
        //  StreamWriter streamWriter = new StreamWriter(fullpath+".new");
        //  Stream stream = streamWriter.BaseStream;
        //  sf.Serialize(stream, thingStencil);
        //  stream.Close();
        //}

        SoapFormatter sf = new SoapFormatter();
        StreamReader streamRdr = new StreamReader(fullpath);
        Stream stream = streamRdr.BaseStream;
        ThingStencil thingStencil = (ThingStencil)sf.Deserialize(stream);
        thingStencil.Tag = Path.GetFileNameWithoutExtension(fullpath);
        m_Config.ThingStencils.Add(Path.GetFileNameWithoutExtension(fullpath), thingStencil);
        stream.Close();
        //Console.WriteLine("  {0}] {1}", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
        LogNote("Srvr", 0, "  %i] %s", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
      }
    }

  }

    private void LogNote(string p, int p_2, string p_3, int p_4, string p_5)
    {
      throw new Exception("The method or operation is not implemented.");
    }

    private void LogNote(string p, int p_2, string p_3)
    {
      throw new Exception("The method or operation is not implemented.");
    }

  void MarshalConfig()
  {
    RemotingServices.Marshal(m_Config, "Global");
  }

  bool ChangeState(ServiceProtocol serviceProtocol, Int64 requestID, BaseProtocol.RunStates runState)
  {
    if (true) // Decide whether to allow runstate change
    { // We're going to do it.
      // Change the runstate.

      // Raise event(s).
      serviceProtocol.DoStateChanged(requestID, runState);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

    void GetPropertyValues(ServiceProtocol serviceProtocol, Int64 requestID, ref ArrayList propertyList)
  {
    // Return modified ArrayList with tag details included.
  }

  void GetSubTags(ServiceProtocol serviceProtocol, Int64 requestID, String propertyPath, out ArrayList propertyList)
  {
    propertyList = new ArrayList();
    Random random = new Random();

    // Return ArrayList with tag details included.
    for (int i=0; i<10; i++)
    {
      StringBuilder builder = new StringBuilder();
      System.Char ch;
      int n = random.Next(5, 10);
      for (int j = 0; j < n; j++)
      {
        ch = Convert.ToChar(random.Next(33, 126));
        builder.Append(ch);
      }
      switch (random.Next(0, 3))
      {
      case 0:
        propertyList.Add(new ModelProperty(builder.ToString(), random.Next()));
        break;
      case 1:
        propertyList.Add(new ModelProperty(builder.ToString(), random.NextDouble()));
        break;
      case 2:
        propertyList.Add(new ModelProperty(builder.ToString(), builder.ToString()));
        break;
      case 3:
        propertyList.Add(new ModelProperty(builder.ToString(), Color.FromArgb(random.Next())));
        break;
      }
    }
  }


  void Sync(ServiceProtocol serviceProtocol, Int64 requestID)
  {
    serviceProtocol.Sync(requestID);
  }

  bool CreateItem(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
  {
    // Need to check for runstate here, and decide if we'll fire DoItemCreated.
    // This is required in case a rogue client tries to create an item even when not supposed to.
    // This applies to all three create*, and all three delete* events.
    if (true) // Decide whether to create an item.
    { // We're going to do it.
      // Create the item.

      // Raise event(s).
      serviceProtocol.DoItemCreated(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool ModifyItem(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
  {
    if (true) // Decide whether to modify an item.
    { // We're going to do it.

      // Modify the item.

      // Need to get hold of a valid pDoc pointer... *********

      //Individual changes would go something like this: *********
      //int length = tag.Length;
      //wchar_t * tagwc = (wchar_t*)(void*)Marshal.StringToHGlobalUni(tag);

      //char * tagc = (char *)malloc(length+1);
      //tagc[length] = 0;
      //for (int i=0; i<length; i++)
      //  tagc[i] = (char)tagwc[i];

      //pDoc.GCB.DoModify(tagc, 
      //                    angle,
      //                    boundingRect, 
      //                    fillColor,
      //                    fillMode,
      //                    mirrorX,
      //                    mirrorY);

      // Raise event(s).
      serviceProtocol.DoItemModified(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool ModifyItemBoundingRect(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, RectangleF boundingRect)
  {
    if (true) // Decide whether to modify an item.
    { // We're going to do it.
      // Modify the item.

      // Raise event(s).
      serviceProtocol.DoItemBoundingRectModified(requestID, guid, boundingRect);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool ModifyItemPath(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String path)
  {
    if (true) // Decide whether to modify an item.
    { // We're going to do it.
      // Modify the item.

      // Raise event(s).
      serviceProtocol.DoItemPathModified(requestID, guid, path);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool DeleteItem(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid)
  {
    if (true) // Decide whether to delete an item.
    { // We're going to do it.
      // Delete the item.

      // Raise event(s).
      serviceProtocol.DoItemDeleted(requestID, guid);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool CreateLink(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
  {
    if (true) // Decide whether to create an link.
    { // We're going to do it.
      // Create the item.

      // Raise event(s).
      serviceProtocol.DoLinkCreated(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool ModifyLink(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
  {
    if (true) // Decide whether to modify an link.
    { // We're going to do it.
      // Modify the item.

      // Raise event(s).
      serviceProtocol.DoLinkModified(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool ModifyLinkControlPoints(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, List<PointF> controlPoints)
  {
    if (true) // Decide whether to modify an item.
    { // We're going to do it.
      // Modify the item.

      // Raise event(s).
      serviceProtocol.DoLinkControlPointsModified(requestID, guid, controlPoints);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool DeleteLink(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid)
  {
    if (true) // Decide whether to delete an link.
    { // We're going to do it.
      // Delete the item.

      // Raise event(s).
      serviceProtocol.DoLinkDeleted(requestID, guid);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool CreateThing(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
  {
    if (true) // Decide whether to create an Thing.
    { // We're going to do it.
      // Create the Thing.

      // Raise event(s).
      serviceProtocol.DoThingCreated(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool ModifyThing(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
  {
    if (true) // Decide whether to modify an Thing.
    { // We're going to do it.
      // Modify the Thing.

      // Raise event(s).
      serviceProtocol.DoThingModified(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool ModifyThingPath(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid, String path)
  {
    if (true) // Decide whether to modify an item.
    { // We're going to do it.
      // Modify the item.

      // Raise event(s).
      serviceProtocol.DoThingPathModified(requestID, guid, path);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  bool DeleteThing(ServiceProtocol serviceProtocol, Int64 requestID, Guid guid)
  {
    if (true) // Decide whether to delete an Thing.
    { // We're going to do it.
      // Delete the Thing.

      // Raise event(s).
      serviceProtocol.DoThingDeleted(requestID, guid);

      return true;
    }
    else
    { // We're not going to do it.
      return false;
    }
  }

  PortStatus PortCheck(ServiceProtocol serviceProtocol, Guid guid, Anchor anchor)
  {
    //		CNSGuidItem * pGuid = new CNSGuidItem();
    //		pGuid.m_Guid = guid;
    //		m_pUnmanaged.m_Guids.AddTail(null);
    //		CNSGuidItem * pGuid = m_pUnmanaged.m_Guids.Find(guid);
    //		CNSMdlLink * pLink = dynamic_cast<CNSMdlLink *>(guid);
    return PortStatus.Available;
  }

  ArrayList PropertyListCheck(ServiceProtocol serviceProtocol, Guid guid, String tag, String path)
  {
    //char* dest = new char[tag.Length+1];
    //strcpy(dest, static_cast<LPCTSTR>(const_cast<void*>(static_cast<const void*>(System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(tag)))));
    //ScdMainWnd().PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)dest);

    ArrayList list = new ArrayList();
    // Generate list of properties at this level in properties tree.
    return list;
  }

  void MarshalServiceInterface()
  {
    ServiceProtocol.ChangeStateHandler changeState = new ServiceProtocol.ChangeStateHandler(ChangeState);

    ServiceProtocol.GetPropertyValuesHandler getPropertyValues = new ServiceProtocol.GetPropertyValuesHandler(GetPropertyValues);
    ServiceProtocol.GetSubTagsHandler getSubTags = new ServiceProtocol.GetSubTagsHandler(GetSubTags);

    ServiceProtocol.CreateItemHandler createItem = new ServiceProtocol.CreateItemHandler(CreateItem);
    ServiceProtocol.ModifyItemHandler modifyItem = new ServiceProtocol.ModifyItemHandler(ModifyItem);
    ServiceProtocol.ModifyItemPathHandler modifyItemPath = new ServiceProtocol.ModifyItemPathHandler(ModifyItemPath);
    ServiceProtocol.DeleteItemHandler deleteItem = new ServiceProtocol.DeleteItemHandler(DeleteItem);

    ServiceProtocol.CreateLinkHandler createLink = new ServiceProtocol.CreateLinkHandler(CreateLink);
    ServiceProtocol.ModifyLinkHandler modifyLink = new ServiceProtocol.ModifyLinkHandler(ModifyLink);
    ServiceProtocol.DeleteLinkHandler deleteLink = new ServiceProtocol.DeleteLinkHandler(DeleteLink);

    ServiceProtocol.CreateThingHandler createThing = new ServiceProtocol.CreateThingHandler(CreateThing);
    ServiceProtocol.ModifyThingHandler modifyThing = new ServiceProtocol.ModifyThingHandler(ModifyThing);
    ServiceProtocol.ModifyThingPathHandler modifyThingPath = new ServiceProtocol.ModifyThingPathHandler(ModifyThingPath);
    ServiceProtocol.DeleteThingHandler deleteThing = new ServiceProtocol.DeleteThingHandler(DeleteThing);

    ServiceProtocol.PortCheckHandler portCheck = new ServiceProtocol.PortCheckHandler(PortCheck);

    ServiceProtocol.PropertyListHandler propertyListCheck = new ServiceProtocol.PropertyListHandler(PropertyListCheck);


    serviceProtocol = new ServiceProtocol(changeState, getPropertyValues, getSubTags, createItem, modifyItem, modifyItemPath, deleteItem, createLink, modifyLink, deleteLink, createThing, modifyThing, modifyThingPath, deleteThing, portCheck, propertyListCheck);

    String filename = PrjName(); //gs_pPrj.

    RemotingServices.Marshal(serviceProtocol, filename);
    m_Config.ProjectList.Add(filename);
    LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
  }

    private void LogNote(string p, int p_2, string p_3, string filename)
    {
      throw new Exception("The method or operation is not implemented.");
    }

    private string PrjName()
    {
      throw new Exception("The method or operation is not implemented.");
    }

  void Startup()
  {
    LogNote("CNETServerThread", 0, "Startup");

    m_Config = new ConfigData();
    m_StencilPath = BaseCfgFiles();
    m_StencilPath = m_StencilPath + "Stencils\\";

    //m_pUnmanaged = new CNETServerU;

    //m_pUnmanaged.Startup();

    Console.WriteLine("Startup");
    CreateConnects();
    GetStencils();
    MarshalServiceInterface();
    MarshalConfig();

    LoadItems();
  }

    private string BaseCfgFiles()
    {
      throw new Exception("The method or operation is not implemented.");
    }

  void Shutdown()
  {
    //delete m_Config;

    LogNote("CNETServerThread", 0, "Shutdown");
  }

  Dictionary<String, PointF> pageOffset;

  void LoadItems()
  {
    //CWaitMsgCursor Wait("9.10");
    String filename = PrjName(); //gs_pPrj.

    //CDocTemplate & Template = ScdApp().GraphTemplate();




    //StringCollection pages = new StringCollection();
    //pageOffset = new Dictionary<String, PointF>();

    //{
    //  POSITION Pos = Template.GetFirstDocPosition();
    //  while (Pos)
    //  {
    //    CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));

    //    String page = new String(pDoc.GetTitle());


    //    if (!pages.Contains(page))
    //    {
    //      pages.Add(page);
    //    }

    //  }

    //  int sqrtPages = (int)Math.Round(Math.Sqrt((double)pages.Count)+0.5);
    //  int i = 0;
    //  int j = 0;
    //  float dX = 0.0F;
    //  float dY = 0.0F;
    //  for (int k=0; k<pages.Count; k++)
    //  {
    //    String page = pages[k];
    //    if (page.Length>4)
    //      page = page.Substring(0,page.Length-4);//remove .scg
    //    pageOffset.Add("/" + filename + "/" + page + "/", PointF(dX, dY));

    //    i++;
    //    dX += 450.0F;
    //    if (i > sqrtPages-1)
    //    {
    //      i = 0;
    //      dX = 0.0F;
    //      j++;
    //      dY += 300.0F;
    //    }
    //  }

    //}















    //POSITION Pos = Template.GetFirstDocPosition();
    //while (Pos)
    //{
    //  CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));

    //  CGrfTagInfoArray GTIA;

    //  //if (OpenSymbolExport())
    //  //  pDoc.GCB.pDrw.SaveSymbols(m_SymbolExportFolder.GetBuffer(), pDoc.m_sDocGUID());
    //  const int Cnt = pDoc.GetTagListInfo(true, GTIA);
    //  if (Cnt>0)
    //  {
    //    String page = new String(pDoc.GetTitle());
    //    Guid guid;
    //    if (page.Length>4)
    //      page = page.Substring(0,page.Length-4);//remove .scg
    //    POSITION pos = pDoc.GetFirstViewPosition();
    //    CView* pFirstView = pDoc.GetNextView( pos );
    //    CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
    //    for (int i=0; i<Cnt; i++)
    //    {
    //      CGrfTagInfo & I = GTIA[i];
    //      //if (I.m_bHasTag && FindOldTag(I.m_sTag())<0)
    //      //  continue;

    //      int TagTyp = -1; //error
    //      if (I.m_bHasTag)
    //      {
    //        FlwNode * pNode = gs_pSfeSrvr.FE_FindNode(I.m_sTag());
    //        if (pNode)
    //        {
    //          TagObjClass * pTagObjC = pNode.Class();
    //          I.m_sClass = pNode.ClassId();
    //          guid = Guid(new String(pNode.EqpGUID()));


    //          //CXM_ObjectTag ObjTag(I.m_sTag(), TABOpt_Parms);
    //          //CXM_ObjectData ObjData;
    //          //CXM_Route Route;
    //          //TagObjClass * pTagObjC = NULL;
    //          //if (pGWnd.XReadTaggedItem(ObjTag, ObjData, Route))
    //          //  {
    //          //  I.m_sClass = ObjData.FirstItem().Class();
    //          //  pTagObjC = TagObjClass.FindClassId(I.m_sClass());
    //          //  if (pTagObjC==NULL)
    //          //    pTagObjC = TagObjClass.FindClassName(I.m_sClass());
    //          if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC.Group())==0 || 
    //            _stricmp(CtrlLinkGrp, pTagObjC.Group())==0 || 
    //            _stricmp(ElecLinkGrp, pTagObjC.Group())==0 || 
    //            _stricmp(AirLinkGrp, pTagObjC.Group())==0))
    //            TagTyp = 2; //link
    //          else
    //            TagTyp = 1; //unit/node
    //          if (pTagObjC)
    //          {
    //            I.m_sDrwGroup=pTagObjC.DrwGroup();
    //          }

    //          //Strng GuidTag(I.m_sTag());
    //          //GuidTag+=".EqpGUID";
    //          //CXM_ObjectTag GuidObjTag(GuidTag(), TABOpt_Parms);
    //          //CXM_ObjectData GuidObjData;
    //          //CXM_Route GuidRoute;
    //          //if (pGWnd.XReadTaggedItem(GuidObjTag, GuidObjData, GuidRoute))
    //          //  {
    //          //  Guid = GuidObjData.FirstItem().Value().GetString();
    //          //  }  
    //        }
    //        else
    //        {
    //          TagTyp=3;
    //          I.m_sClass = "";
    //        }

    //        if (TagTyp==2)
    //        {
    //          CLinePointsArray LPA;
    //          pDoc.GCB.pDrw.CollectLinkInfo(I, LPA);

    //          GraphicLink graphicLink = new GraphicLink(guid, new String(I.m_sTag()), new String(I.m_sClass()), Guid(new String(pNode.Nd_Rmt(0).EqpGUID())), new String(pNode.IOArea_Rmt(0).m_pName), Guid(new String(pNode.Nd_Rmt(1).EqpGUID())), new String(pNode.IOArea_Rmt(1).m_pName));

    //          graphicLink.ControlPoints = new List<PointF>();
    //          for (int i=0; i<LPA.GetSize(); i++)
    //            graphicLink.ControlPoints.Add(PointF(LPA[i].x, -LPA[i].y));

    //          serviceProtocol.graphicLinks.Add(graphicLink.Guid, graphicLink);
    //        }
    //        else if (TagTyp==1)// || TagTyp==0)
    //        {
    //          Strng tag = I.m_sTag();

    //          if (tag.Find("FLOWSHEET_")) // _NOT_ a FLOWSHEET_* non-unit -- There must be a better way to find/handle these?
    //          {
    //            GraphicItem graphicItem = new GraphicItem(guid, new String(I.m_sTag()));
    //            String path = "/" + filename + "/" + page + "/";

    //            float left = (float)I.m_LoBnd.m_X + pageOffset[path].X;
    //            float top = -(float)I.m_HiBnd.m_Y + pageOffset[path].Y;
    //            float width = (float)I.m_HiBnd.m_X - (float)I.m_LoBnd.m_X;
    //            float height = (float)I.m_HiBnd.m_Y - (float)I.m_LoBnd.m_Y;

    //            float rotation  = -(float)I.m_Node.m_Rotation;

    //            PointF d = pageOffset[path];

    //            graphicItem.Populate(filename, page,
    //              guid.ToString(), new String(I.m_sClass()), 
    //              RectangleF(left, top, width, height),
    //              rotation);

    //            serviceProtocol.graphicItems.Add(graphicItem.Guid, graphicItem);
    //          }
    //          else
    //          {
    //            // This creates a dummy 'tank-1' thing annotation.
    //            //DoneOne = true;
    //            //CNSGrfThing * pGrf=new CNSGrfThing(I.m_sTag(), Guid(), Page(), I);
    //            //m_Things.AddTail(pGrf);
    //          }
    //        }
    //      }
    //      //if (TagTyp==3)  
    //      //  {
    //      //  I.m_sTag.Set("$%i$", ++InsertCnt);
    //      //  I.m_sClass="Insert";
    //      //  I.m_sDrwGroup="Inserts";

    //      //  DoneOne=true;
    //      //  WriteInsert(m_sGroup, I, m_Origin, Page(), pDoc.m_sDocGUID());
    //      //  ExportSymbol(I, m_Origin, pDoc.m_sDocGUID());
    //      //  }
    //    }
    //    //if (m_bWithGroups && DoneOne)
    //    //  WriteGroupInfo(m_sGroup, m_sGroupDesc, m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
    //  }

    //  //CloseSymbolExport();


    //  //  RemotingServices.Marshal(graphic, filename);
    //  //  m_Config.projectList.Add(filename);
    //  //  LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
    //}

    //{
    //  foreach (GraphicLink graphicLink in serviceProtocol.graphicLinks.Values)
    //  {
    //    PointF offset = PointF.Empty;
    //    {
    //      String path = "";

    //      Guid origin = graphicLink.Origin;
    //      Guid destination = graphicLink.Destination;

    //      if (origin != Guid.Empty)
    //        path = serviceProtocol.graphicItems[origin].Path;
    //      else if (destination != Guid.Empty)
    //        path = serviceProtocol.graphicItems[destination].Path;

    //      if (path.Length>0)
    //        offset = pageOffset[path];
    //    }

    //    List<PointF> oldControlPoints = graphicLink.ControlPoints;
    //    List<PointF> newControlPoints = new List<PointF>();

    //    foreach (PointF point in oldControlPoints)
    //    {
    //      point.X += offset.X;
    //      point.Y += offset.Y;
    //      newControlPoints.Add(point);
    //    }
    //    graphicLink.ControlPoints = newControlPoints;
    //  }
    //}
  }

    
  void SaveItems()
  {
    //Assmue nothign is deleted, created or re-pathed!
    //Also (consequently) assuming that pageOffset is correct and up to date.
      //It should have been filled properly at project load...

    //CWaitMsgCursor Wait("10.9");
    String filename = PrjName(); //gs_pPrj.

    //CDocTemplate & Template = ScdApp().GraphTemplate();

    //POSITION Pos = Template.GetFirstDocPosition();
    //while (Pos)
    //{
    //  CGrfDoc * pDoc=dynamic_cast<CGrfDoc*>(Template.GetNextDoc(Pos));

    //  CGrfTagInfoArray GTIA;

    //  //if (OpenSymbolExport())
    //  //  pDoc.GCB.pDrw.SaveSymbols(m_SymbolExportFolder.GetBuffer(), pDoc.m_sDocGUID());
    //  const int Cnt = pDoc.GetTagListInfo(true, GTIA);
    //  if (Cnt>0)
    //  {
    //    String page = new String(pDoc.GetTitle());
    //    Guid guid;
    //    if (page.Length>4)
    //      page = page.Substring(0,page.Length-4);//remove .scg
    //    POSITION pos = pDoc.GetFirstViewPosition();
    //    CView* pFirstView = pDoc.GetNextView( pos );
    //    CGrfWnd* pGWnd = (CGrfWnd*)pFirstView;
    //    for (int i=0; i<Cnt; i++)
    //    {
    //      CGrfTagInfo & I = GTIA[i];
    //      //if (I.m_bHasTag && FindOldTag(I.m_sTag())<0)
    //      //  continue;

    //      int TagTyp = -1; //error
    //      if (I.m_bHasTag)
    //      {
    //        FlwNode * pNode = gs_pSfeSrvr.FE_FindNode(I.m_sTag());
    //        if (pNode)
    //        {
    //          TagObjClass * pTagObjC = pNode.Class();
    //          I.m_sClass = pNode.ClassId();
    //          guid = Guid(new String(pNode.EqpGUID()));


    //          //CXM_ObjectTag ObjTag(I.m_sTag(), TABOpt_Parms);
    //          //CXM_ObjectData ObjData;
    //          //CXM_Route Route;
    //          //TagObjClass * pTagObjC = NULL;
    //          //if (pGWnd.XReadTaggedItem(ObjTag, ObjData, Route))
    //          //  {
    //          //  I.m_sClass = ObjData.FirstItem().Class();
    //          //  pTagObjC = TagObjClass.FindClassId(I.m_sClass());
    //          //  if (pTagObjC==NULL)
    //          //    pTagObjC = TagObjClass.FindClassName(I.m_sClass());
    //          if (pTagObjC && (_stricmp(FlwLinkGrp, pTagObjC.Group())==0 || 
    //            _stricmp(CtrlLinkGrp, pTagObjC.Group())==0 || 
    //            _stricmp(ElecLinkGrp, pTagObjC.Group())==0 || 
    //            _stricmp(AirLinkGrp, pTagObjC.Group())==0))
    //            TagTyp = 2; //link
    //          else
    //            TagTyp = 1; //unit/node
    //          if (pTagObjC)
    //          {
    //            I.m_sDrwGroup=pTagObjC.DrwGroup();
    //          }

    //          //Strng GuidTag(I.m_sTag());
    //          //GuidTag+=".EqpGUID";
    //          //CXM_ObjectTag GuidObjTag(GuidTag(), TABOpt_Parms);
    //          //CXM_ObjectData GuidObjData;
    //          //CXM_Route GuidRoute;
    //          //if (pGWnd.XReadTaggedItem(GuidObjTag, GuidObjData, GuidRoute))
    //          //  {
    //          //  Guid = GuidObjData.FirstItem().Value().GetString();
    //          //  }  
    //        }
    //        else
    //        {
    //          TagTyp=3;
    //          I.m_sClass = "";
    //        }

    //        if (TagTyp==2)
    //        {
    //          CLinePointsArray LPA;
    //          pDoc.GCB.pDrw.CollectLinkInfo(I, LPA);

    //          //GraphicLink graphicLink = new GraphicLink(guid, new String(I.m_sTag()), new String(I.m_sClass()), Guid(new String(pNode.Nd_Rmt(0).EqpGUID())), new String(pNode.IOArea_Rmt(0).m_pName), Guid(new String(pNode.Nd_Rmt(1).EqpGUID())), new String(pNode.IOArea_Rmt(1).m_pName));

    //          //graphicLink.ControlPoints = new List<PointF>();
    //          //for (int i=0; i<LPA.GetSize(); i++)
    //          //  graphicLink.ControlPoints.Add(PointF(LPA[i].x, -LPA[i].y));

    //          //serviceProtocol.graphicLinks.Add(graphicLink.Guid, graphicLink);
    //        }
    //        else if (TagTyp==1)// || TagTyp==0)
    //        {
    //          Strng tag = I.m_sTag();

    //          if (tag.Find("FLOWSHEET_")) // _NOT_ a FLOWSHEET_* non-unit -- There must be a better way to find/handle these?
    //          {
    //            GraphicItem graphicItem = serviceProtocol.graphicItems[guid];

    //            // This is the code for converting the String.char*, 
    //            // and then the dummy call to modify the item.  *********

    //            // The structure of the above has been changed many times, 
    //            // and possibly (probably?) no longer produces the correct pDoc...
    //            // One solution (temporary) if that is difficult is to process _all_
    //            // items once for each pDoc, and have the below function drop out 
    //            // if tagc isn't found in the pDoc.

    //            int length = graphicItem.Tag.Length;
    //            wchar_t * tagwc = (wchar_t*)(void*)Marshal.StringToHGlobalUni(graphicItem.Tag);

    //            char * tagc = (char *)malloc(length+1);
    //            tagc[length] = 0;
    //            for (int i=0; i<length; i++)
    //              tagc[i] = (char)tagwc[i];

    //            pDoc.GCB.DoModify(tagc, 
    //                                graphicItem.Angle,
    //                                graphicItem.BoundingRect.X, 
    //                                graphicItem.BoundingRect.Y, 
    //                                graphicItem.BoundingRect.Width, 
    //                                graphicItem.BoundingRect.Height, 
    //                                graphicItem.FillColor.ToArgb(),
    //                                graphicItem.MirrorX,
    //                                graphicItem.MirrorY);


    //            //pDoc.GCB.pDrw.TranslateEntity(Like, pDsp.StartPt.World, pDsp.CurrentPt.World);
    //            //gs_pCmd.ExtendCmdLine(";"); //SelectionList will be clear or invalid, cannot continue repositioning !!!
    //            //pDrw.EntityInvalidate(Like, NULL);
    //            //pDsp.Draw(Like, -1);basdffruyygiuh;oi7

    //            //I.m_LoBnd.m_X = graphicItem.BoundingRect.Left - pageOffset[graphicItem.Path].X;
    //            //I.m_HiBnd.m_Y = -(graphicItem.BoundingRect.Top - pageOffset[graphicItem.Path].Y);
    //            //I.m_HiBnd.m_X = (float)I.m_LoBnd.m_X + graphicItem.BoundingRect.Width;
    //            //I.m_LoBnd.m_Y = (float)I.m_HiBnd.m_Y - graphicItem.BoundingRect.Height;

    //            //I.m_Node.m_Rotation = -graphicItem.Angle;
    //          }
    //          else
    //          {
    //            // This creates a dummy 'tank-1' thing annotation.
    //            //DoneOne = true;
    //            //CNSGrfThing * pGrf=new CNSGrfThing(I.m_sTag(), Guid(), Page(), I);
    //            //m_Things.AddTail(pGrf);
    //          }
    //        }
    //      }
    //      //if (TagTyp==3)  
    //      //  {
    //      //  I.m_sTag.Set("$%i$", ++InsertCnt);
    //      //  I.m_sClass="Insert";
    //      //  I.m_sDrwGroup="Inserts";

    //      //  DoneOne=true;
    //      //  WriteInsert(m_sGroup, I, m_Origin, Page(), pDoc.m_sDocGUID());
    //      //  ExportSymbol(I, m_Origin, pDoc.m_sDocGUID());
    //      //  }
    //    }
    //    //if (m_bWithGroups && DoneOne)
    //    //  WriteGroupInfo(m_sGroup, m_sGroupDesc, m_Origin, m_Lo, m_Hi, m_AlignLo, m_AlignHi);
    //  }

    //  //CloseSymbolExport();


    //  //  RemotingServices.Marshal(graphic, filename);
    //  //  m_Config.projectList.Add(filename);
    //  //  LogNote("Srvr", 0, "Added project %s to ProjectList.", filename);
    //}
  }













  void Update9_10()
  {
    serviceProtocol.graphicItems.Clear();
    serviceProtocol.graphicLinks.Clear();
    serviceProtocol.graphicThings.Clear();
    LoadItems();
    serviceProtocol.Sync(0);
  }

  void Update10_9()
  {
    SaveItems();
  }
    }
}
