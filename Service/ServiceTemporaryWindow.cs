using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Protocol;
using System.Collections;
using System.IO;
using System.Runtime.Remoting.Channels;
using System.Runtime.Serialization.Formatters.Soap;
using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting;
using SysCAD.Log;
using System.Runtime.Serialization.Formatters.Binary;

namespace Service
{
  public partial class ServiceTemporaryWindow : Form, ILog
  {
    public ConfigData configData;

    public Dictionary<Guid, GraphicGroup> graphicGroups;

    public Dictionary<Guid, GraphicItem> graphicItems;
    public Dictionary<Guid, GraphicLink> graphicLinks;
    public Dictionary<Guid, GraphicThing> graphicThings;

    ClientServiceProtocol clientClientServiceProtocol;
    EngineServiceProtocol engineClientServiceProtocol;

    private String projectName;

    private String projectPath;
    private String configPath;
    private String stencilPath;

    private Int64 requestId;
    private Int64 eventId;

    
    public ServiceTemporaryWindow(String projectPath, String configPath, String stencilPath)
    {
      InitializeComponent();

      System.Runtime.Remoting.Channels.BinaryServerFormatterSinkProvider serverProv = new BinaryServerFormatterSinkProvider();
      serverProv.TypeFilterLevel = System.Runtime.Serialization.Formatters.TypeFilterLevel.Full;

      BinaryClientFormatterSinkProvider clientProv = new BinaryClientFormatterSinkProvider();

      Hashtable ipcProps = new Hashtable();
      ipcProps["portName"] = "SysCAD.Service";
      //ipcProps["typeFilterLevel"] = TypeFilterLevel.Full;
      IpcChannel ipcChannel = new IpcChannel(ipcProps, clientProv, serverProv);
      ChannelServices.RegisterChannel(ipcChannel, false);

      projectName = Path.GetFileNameWithoutExtension(Path.GetDirectoryName(projectPath));

      this.projectPath = projectPath;
      this.configPath = configPath;
      this.stencilPath = stencilPath;

      logView.LogFile = projectPath + "test.log";

      configData = new ConfigData();
      GetStencils(configData);
      configData.ProjectList.Add(projectName);
      RemotingServices.Marshal(configData, "Global");

      graphicGroups = new Dictionary<Guid, GraphicGroup>();

      graphicItems = new Dictionary<Guid, GraphicItem>();
      graphicLinks = new Dictionary<Guid, GraphicLink>();
      graphicThings = new Dictionary<Guid, GraphicThing>();

      LoadGraphics();

      ClientServiceProtocol.LoadHandler engineLoad = new ClientServiceProtocol.LoadHandler(LoadGraphics);
      ClientServiceProtocol.SaveHandler engineSave = new ClientServiceProtocol.SaveHandler(SaveGraphics);

      ClientServiceProtocol.ChangePermissionsHandler clientChangePermissions = new ClientServiceProtocol.ChangePermissionsHandler(ChangePermissions);

      ClientServiceProtocol.GetPropertyValuesHandler clientGetPropertyValues = new ClientServiceProtocol.GetPropertyValuesHandler(GetPropertyValues);
      ClientServiceProtocol.GetSubTagsHandler clientGetSubTags = new ClientServiceProtocol.GetSubTagsHandler(GetSubTags);

      ClientServiceProtocol.CreateGroupHandler clientCreateGroup = new ClientServiceProtocol.CreateGroupHandler(CreateGroup);
      ClientServiceProtocol.ModifyGroupHandler clientModifyGroup = new ClientServiceProtocol.ModifyGroupHandler(ModifyGroup);
      ClientServiceProtocol.DeleteGroupHandler clientDeleteGroup = new ClientServiceProtocol.DeleteGroupHandler(DeleteGroup);

      ClientServiceProtocol.CreateItemHandler clientCreateItem = new ClientServiceProtocol.CreateItemHandler(CreateItem);
      ClientServiceProtocol.ModifyItemHandler clientModifyItem = new ClientServiceProtocol.ModifyItemHandler(ModifyItem);
      ClientServiceProtocol.ModifyItemPathHandler clientModifyItemPath = new ClientServiceProtocol.ModifyItemPathHandler(ModifyItemPath);
      ClientServiceProtocol.DeleteItemHandler clientDeleteItem = new ClientServiceProtocol.DeleteItemHandler(DeleteItem);

      ClientServiceProtocol.CreateLinkHandler clientCreateLink = new ClientServiceProtocol.CreateLinkHandler(CreateLink);
      ClientServiceProtocol.ModifyLinkHandler clientModifyLink = new ClientServiceProtocol.ModifyLinkHandler(ModifyLink);
      ClientServiceProtocol.DeleteLinkHandler clientDeleteLink = new ClientServiceProtocol.DeleteLinkHandler(DeleteLink);

      ClientServiceProtocol.CreateThingHandler clientCreateThing = new ClientServiceProtocol.CreateThingHandler(CreateThing);
      ClientServiceProtocol.ModifyThingHandler clientModifyThing = new ClientServiceProtocol.ModifyThingHandler(ModifyThing);
      ClientServiceProtocol.ModifyThingPathHandler clientModifyThingPath = new ClientServiceProtocol.ModifyThingPathHandler(ModifyThingPath);
      ClientServiceProtocol.DeleteThingHandler clientDeleteThing = new ClientServiceProtocol.DeleteThingHandler(DeleteThing);

      ClientServiceProtocol.PortCheckHandler clientPortCheck = new ClientServiceProtocol.PortCheckHandler(PortCheck);

      ClientServiceProtocol.PropertyListHandler clientPropertyListCheck = new ClientServiceProtocol.PropertyListHandler(PropertyListCheck);

      ClientServiceProtocol.LogMessageHandler clientLogMessage = new ClientServiceProtocol.LogMessageHandler(LogMessage);

      clientClientServiceProtocol = new ClientServiceProtocol(projectName,
                                                              engineLoad, engineSave,
                                                              graphicGroups, graphicLinks, graphicItems, graphicThings,
                                                              clientChangePermissions, clientGetPropertyValues, clientGetSubTags,
                                                              clientCreateGroup, clientModifyGroup, clientDeleteGroup,
                                                              clientCreateItem, clientModifyItem, clientModifyItemPath, clientDeleteItem,
                                                              clientCreateLink, clientModifyLink, clientDeleteLink,
                                                              clientCreateThing, clientModifyThing, clientModifyThingPath,
                                                              clientDeleteThing, clientPortCheck, clientPropertyListCheck, clientLogMessage);


      RemotingServices.Marshal(clientClientServiceProtocol, "Client/" + projectName);

      EngineServiceProtocol.LogMessageHandler engineLogMessage = new EngineServiceProtocol.LogMessageHandler(LogMessage);
      EngineServiceProtocol.StateChangedHandler engineStateChanged = new EngineServiceProtocol.StateChangedHandler(StateChanged);

      engineClientServiceProtocol = new EngineServiceProtocol(projectName,
                                                              graphicGroups, graphicLinks, graphicItems, graphicThings, 
                                                              engineLogMessage, engineStateChanged);

      RemotingServices.Marshal(engineClientServiceProtocol, "Engine/" + projectName);
    }

    bool ChangePermissions(out Int64 requestId, ClientBaseProtocol.Permissions permissions)
    {
      if (true) // Decide whether to allow runstate change
      { // We're going to do it.
        // Change the runstate.

        this.requestId++;
        requestId = this.requestId;
        throw new NotImplementedException("The method or operation is not implemented.");

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoPermissionsChanged(eventId, requestId, permissions);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool CreateGroup(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect)
    {
      // Need to check for runstate here, and decide if we'll fire DoGroupCreated.
      // This is required in case a rogue client tries to create an Group even when not supposed to.
      // This applies to all three create*, and all three delete* events.
      if (true)
      { // We're going to do it.
        // Create the Group.
        this.requestId++;
        requestId = this.requestId;
        guid = Guid.NewGuid();

        GraphicGroup graphicGroup = new GraphicGroup(guid, tag);
        graphicGroup.Path = path;
        graphicGroup.BoundingRect = (ARectangleF)boundingRect;

        graphicGroups.Add(guid, graphicGroup);

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoGroupCreated(eventId, requestId, guid, tag, path, boundingRect);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      // Need to check for runstate here, and decide if we'll fire DoItemCreated.
      // This is required in case a rogue client tries to create an item even when not supposed to.
      // This applies to all three create*, and all three delete* events.
      if (true)
      { // We're going to do it.
        // Create the item.
        this.requestId++;
        requestId = this.requestId;
        guid = Guid.NewGuid();

        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = (ARectangleF)boundingRect;
        graphicItem.Angle = angle;
        graphicItem.TextArea = textArea;
        graphicItem.FillColor = fillColor;
        graphicItem.FillMode = fillMode;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        // Raise event(s).
        clientClientServiceProtocol.DoItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, fillMode, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool CreateLink(out Int64 requestId, out Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (true) // Decide whether to create an link.
      { // We're going to do it.
        // Create the item.

        this.requestId++;
        requestId = this.requestId;
        guid = new Guid();

        GraphicLink graphicLink = new GraphicLink(guid, tag, classID, origin, originPort, destination, destinationPort, controlPoints);
        graphicLinks.Add(guid, graphicLink);

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoLinkCreated(eventId, requestId, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (true) // Decide whether to create an Thing.
      { // We're going to do it.
        // Create the Thing.

        this.requestId++;
        requestId = this.requestId;
        guid = Guid.NewGuid();

        throw new NotImplementedException("The method or operation is not implemented.");

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool DeleteGroup(out Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    bool DeleteItem(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicItems.ContainsKey(guid))
      { // We're going to do it.

        // Delete the item.
        graphicItems.Remove(guid);

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoItemDeleted(eventId, requestId, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool DeleteLink(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicLinks.ContainsKey(guid))
      { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoLinkDeleted(eventId, requestId, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool DeleteThing(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicThings.ContainsKey(guid))
      { // We're going to do it.
        // Delete the Thing.

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoThingDeleted(eventId, requestId, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    void GetPropertyValues(out Int64 requestId, ref ArrayList propertyList)
    {
      this.requestId++;
      requestId = this.requestId;
      throw new NotImplementedException("The method or operation is not implemented.");
      // Return modified ArrayList with tag details included.
    }

    void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      this.requestId++;
      requestId = this.requestId;

      propertyList = new ArrayList();
      Random random = new Random();

      // Return ArrayList with tag details included.
      for (int i = 0; i < 10; i++)
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

    bool ModifyGroup(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, RectangleF textArea, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicItems.ContainsKey(guid))
      { // We're going to do it.

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

        // Modify the item.
        GraphicItem graphicItem = graphicItems[guid];

        graphicItem.Tag = tag;
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = (ARectangleF)boundingRect;
        graphicItem.Angle = angle;
        graphicItem.TextArea = textArea;
        graphicItem.FillColor = fillColor;
        graphicItem.FillMode = fillMode;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, fillColor, fillMode, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ModifyItemPath(out Int64 requestId, Guid guid, String path)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicItems.ContainsKey(guid))
      { // We're going to do it.
        // Modify the item.

        GraphicItem graphicItem = graphicItems[guid];

        graphicItem.Path = path;

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoItemPathModified(eventId, requestId, guid, path);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicLinks.ContainsKey(guid))
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoLinkModified(eventId, requestId, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicThings.ContainsKey(guid))
      { // We're going to do it.
        // Modify the Thing.

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ModifyThingPath(out Int64 requestId, Guid guid, String path)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicThings.ContainsKey(guid))
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        eventId++;
        clientClientServiceProtocol.DoThingPathModified(eventId, requestId, guid, path);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    PortStatus PortCheck(out Int64 requestId, Guid guid, Anchor anchor)
    {
      this.requestId++;
      requestId = this.requestId;

      if (graphicItems.ContainsKey(guid))
        return PortStatus.Available;

      else
        return PortStatus.Unavailable;
    }

    ArrayList PropertyListCheck(out Int64 requestId, Guid guid, String tag, String path)
    {
      this.requestId++;
      requestId = this.requestId;

      //char* dest = new char[tag.Length+1];
      //strcpy(dest, static_cast<LPCTSTR>(const_cast<void*>(static_cast<const void*>(System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(tag)))));
      //ScdMainWnd().PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)dest);

      ArrayList list = new ArrayList();
      // Generate list of properties at this level in properties tree.
      return list;
    }

    void LogMessage(out Int64 requestId, String message, MessageType messageType)
    {
      this.requestId++;
      requestId = this.requestId;

      logView.Message(message, messageType);
    }

    bool StateChanged(out Int64 requestId, EngineBaseProtocol.RunState runState)
    {
      if (runState != null)
      {
        this.requestId++;
        requestId = this.requestId;

        ClientBaseProtocol.Permissions permissions = null;
        switch (runState)
        {
          case EngineBaseProtocol.RunState.Edit:
            permissions = new ClientBaseProtocol.Permissions(true, true, true);
            break;
          case EngineBaseProtocol.RunState.Idle:
            permissions = new ClientBaseProtocol.Permissions(false, true, false);
            break;
          case EngineBaseProtocol.RunState.Run:
            permissions = new ClientBaseProtocol.Permissions(false, true, false);
            break;
        }

        eventId++;
        clientClientServiceProtocol.PermissionsChanged(eventId, requestId, permissions);
        return true;
      }
      {
        return false;
      }
    }




    bool ConfirmModelStencil(ModelStencil modelstencil)
    {
      // TODO: check whether this stencil is to be included in the project.
      return true;
    }

    bool LoadGraphics()
    {
      try
      {
        Dictionary<Guid, GraphicGroup> tempGraphicGroups;

        Dictionary<Guid, GraphicLink> tempGraphicLinks;
        Dictionary<Guid, GraphicItem> tempGraphicItems;
        Dictionary<Guid, GraphicThing> tempGraphicThings;

        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamReader streamRdr = new StreamReader(projectPath + "GraphicGroups.10");
          Stream stream = streamRdr.BaseStream;
          tempGraphicGroups = (Dictionary<Guid, GraphicGroup>)bf.Deserialize(stream);
          stream.Close();
        }

        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamReader streamRdr = new StreamReader(projectPath + "GraphicLinks.10");
          Stream stream = streamRdr.BaseStream;
          tempGraphicLinks = (Dictionary<Guid, GraphicLink>)bf.Deserialize(stream);
          stream.Close();
        }

        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamReader streamRdr = new StreamReader(projectPath + "GraphicItems.10");
          Stream stream = streamRdr.BaseStream;
          tempGraphicItems = (Dictionary<Guid, GraphicItem>)bf.Deserialize(stream);
          stream.Close();
        }

        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamReader streamRdr = new StreamReader(projectPath + "GraphicThings.10");
          Stream stream = streamRdr.BaseStream;
          tempGraphicThings = (Dictionary<Guid, GraphicThing>)bf.Deserialize(stream);
          stream.Close();
        }

        graphicGroups = tempGraphicGroups;

        graphicLinks = tempGraphicLinks;
        graphicItems = tempGraphicItems;
        graphicThings = tempGraphicThings;

        if (graphicGroups == null)
          graphicGroups = new Dictionary<Guid, GraphicGroup>();

        if (graphicLinks == null)
          graphicLinks = new Dictionary<Guid, GraphicLink>();
        if (graphicItems == null)
          graphicItems = new Dictionary<Guid, GraphicItem>();
        if (graphicThings == null)
          graphicThings = new Dictionary<Guid, GraphicThing>();

        return true;
      }
      catch (Exception)
      {
        if (graphicGroups == null)
          graphicGroups = new Dictionary<Guid, GraphicGroup>();

        if (graphicLinks == null)
          graphicLinks = new Dictionary<Guid, GraphicLink>();
        if (graphicItems == null)
          graphicItems = new Dictionary<Guid, GraphicItem>();
        if (graphicThings == null)
          graphicThings = new Dictionary<Guid, GraphicThing>();

        return false;
      }
    }

    bool SaveGraphics()
    {
      try
      {
        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamWriter streamWriter = new StreamWriter(projectPath + "GraphicGroups.10");
          Stream stream = streamWriter.BaseStream;
          bf.Serialize(stream, graphicGroups);
          stream.Close();
        }

        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamWriter streamWriter = new StreamWriter(projectPath + "GraphicLinks.10");
          Stream stream = streamWriter.BaseStream;
          bf.Serialize(stream, graphicLinks);
          stream.Close();
        }

        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamWriter streamWriter = new StreamWriter(projectPath + "GraphicItems.10");
          Stream stream = streamWriter.BaseStream;
          bf.Serialize(stream, graphicItems);
          stream.Close();
        }

        {
          BinaryFormatter bf = new BinaryFormatter();
          StreamWriter streamWriter = new StreamWriter(projectPath + "GraphicThings.10");
          Stream stream = streamWriter.BaseStream;
          bf.Serialize(stream, graphicThings);
          stream.Close();
        }
        return true;
      }
      catch (Exception e)
      {
        return false;
      }
    }

    bool LoadGraphics(out Int64 requestId)
    {
      this.requestId++;
      requestId = this.requestId;

      return LoadGraphics();
    }

    bool SaveGraphics(out Int64 requestId)
    {
      this.requestId++;
      requestId = this.requestId;

      return SaveGraphics();
    }

    void GetStencils(ConfigData configData)
    {
      {
        int iStencil = 0;
        LogNote("Srvr", 0, "ModelStencils:");
        String[] dirs = Directory.GetFiles(stencilPath, "*.modelstencil");
        for (int i = 0; i < dirs.GetLength(0); i++)
        {
          String fullpath = dirs[i];

          ////Create dummy ModelStencil for comparison...
          //{
          //  ModelStencil modelStencil2 = new ModelStencil();
          //  modelStencil2.Tag = "";
          //  modelStencil2.GroupName = "Control";
          //  ArrayList elements = new ArrayList();
          //  SysCAD.Protocol.Arc arc = new SysCAD.Protocol.Arc(0, 0, 100, 100, 10, 360);
          //  elements.Add(arc);
          //  modelStencil2.Elements = elements;

          //  modelStencil2.Decorations = new ArrayList();
          //  ArrayList anchors = new ArrayList();
          //  Anchor anchor = new Anchor("tag", AnchorType.Electrical, 0.0f, 1.0f);
          //  anchors.Add(anchor);
          //  modelStencil2.Anchors = anchors;
          //  modelStencil2.FillMode = System.Drawing.Drawing2D.FillMode.Alternate;

          //  SoapFormatter sf2 = new SoapFormatter();
          //  StreamWriter streamWriter = new StreamWriter(fullpath + ".new");
          //  Stream stream2 = streamWriter.BaseStream;
          //  sf2.Serialize(stream2, modelStencil2);
          //  stream2.Close();
          //}

          SoapFormatter sf = new SoapFormatter();
          StreamReader streamRdr = new StreamReader(fullpath);
          Stream stream = streamRdr.BaseStream;
          ModelStencil modelStencil = (ModelStencil)sf.Deserialize(stream);
          modelStencil.Tag = Path.GetFileNameWithoutExtension(fullpath);

          if (ConfirmModelStencil(modelStencil))
          {
            TrimAnchorPoints(modelStencil);
            configData.ModelStencils.Add(Path.GetFileNameWithoutExtension(fullpath), modelStencil);
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
        String[] dirs = Directory.GetFiles(stencilPath, "*.graphicstencil");
        for (int i = 0; i < dirs.GetLength(0); i++)
        {
          String fullpath = dirs[i];

          ////Create dummy GraphicStencil for comparison...
          //{
          //  GraphicStencil graphicStencil2 = new GraphicStencil();
          //  graphicStencil2.Tag = "";
          //  ArrayList elements = new ArrayList();
          //  SysCAD.Protocol.Arc arc = new SysCAD.Protocol.Arc(0, 0, 100, 100, 10, 360);
          //  elements.Add(arc);
          //  graphicStencil2.Elements = elements;

          //  graphicStencil2.Decorations = new ArrayList();

          //  SoapFormatter sf2 = new SoapFormatter();
          //  StreamWriter streamWriter = new StreamWriter(fullpath + ".new");
          //  Stream stream2 = streamWriter.BaseStream;
          //  sf2.Serialize(stream2, graphicStencil2);
          //  stream2.Close();
          //}

          SoapFormatter sf = new SoapFormatter();
          Stream stream = (new StreamReader(fullpath)).BaseStream;
          GraphicStencil graphicStencil = (GraphicStencil)sf.Deserialize(stream);
          stream.Close();


          graphicStencil.Tag = Path.GetFileNameWithoutExtension(fullpath);
          configData.GraphicStencils.Add(Path.GetFileNameWithoutExtension(fullpath), graphicStencil);
          //Console.WriteLine("  {0}] {1}", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
          LogNote("Srvr", 0, "  %i] %s", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
        }
      }

      {
        int iStencil = 0;
        LogNote("Srvr", 0, "ThingStencils:");
        String[] dirs = Directory.GetFiles(stencilPath, "*.thingstencil");
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
          configData.ThingStencils.Add(Path.GetFileNameWithoutExtension(fullpath), thingStencil);
          stream.Close();
          //Console.WriteLine("  {0}] {1}", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
          LogNote("Srvr", 0, "  %i] %s", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
        }
      }

    }

    private void LogNote(string p, int p_2, string p_3)
    {
      Console.WriteLine(p + " : " + p_2 + " : " + p_3);
    }

    private void LogNote(string p, int p_2, string p_3, int p_4, string p_5)
    {
      Console.WriteLine(p + " : " + p_2 + " : " + p_3 + " : " + p_4 + " : " + p_5);
    }

    void TrimAnchorPoints(ModelStencil modelStencil)
    {
      ArrayList anchors = modelStencil.Anchors;
      ArrayList validAnchors = new ArrayList();
      for (int i = 0; i < anchors.Count; i++)
      {
        Anchor anchor = (Anchor)(anchors[i]);
        if (ValidAnchor(anchor))
        {
          validAnchors.Add(anchor);
        }
      }

      modelStencil.Anchors = validAnchors;
    }

    bool ValidAnchor(Anchor anchor)
    {
      // TODO: Check here if anchor.Tag is to be included. (and possibly in future anchor.Type)
      return true;
    }

    public void Message(string msg, MessageType msgType, MessageSource src)
    {
      logView.Message(msg, msgType, src);
    }

    public void Message(string msg, MessageType msgType)
    {
      logView.Message(msg, msgType);
    }

    public void SetSource(MessageSource src)
    {
      logView.SetSource(src);
    }

    public void RemoveSource()
    {
      logView.RemoveSource();
    }

    public bool Active
    {
      get { return logView.Enabled; }
      set { logView.Enabled = value; }
    }

  }
}
