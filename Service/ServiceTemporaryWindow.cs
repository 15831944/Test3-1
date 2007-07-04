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

namespace Service
{
  public partial class ServiceTemporaryWindow : Form
  {

    public ConfigData configData; 
    public Dictionary<Guid, GraphicItem> graphicItems;

    public Dictionary<Guid, GraphicLink> graphicLinks;
    public Dictionary<Guid, GraphicThing> graphicThings;


    ClientServiceProtocol clientClientServiceProtocol;
    private String configPath;
    EngineServiceProtocol engineClientServiceProtocol;

    private String name;
    private String projectPath;
    private String stencilPath;





    
    
    
    
    
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


      
      this.projectPath = projectPath;
      this.configPath = configPath;
      this.stencilPath = stencilPath;

      name = Path.GetFileNameWithoutExtension(Path.GetDirectoryName(projectPath));




      configData = new ConfigData();
      GetStencils(configData);
      configData.ProjectList.Add(name);
      RemotingServices.Marshal(configData, "Global");


      graphicItems = new Dictionary<Guid, GraphicItem>();
      graphicLinks = new Dictionary<Guid,GraphicLink>();
      graphicThings = new Dictionary<Guid,GraphicThing>();


      
      ClientServiceProtocol.ChangeStateHandler clientChangeState = new ClientServiceProtocol.ChangeStateHandler(ClientChangeState);

      ClientServiceProtocol.GetPropertyValuesHandler clientGetPropertyValues = new ClientServiceProtocol.GetPropertyValuesHandler(ClientGetPropertyValues);
      ClientServiceProtocol.GetSubTagsHandler clientGetSubTags = new ClientServiceProtocol.GetSubTagsHandler(ClientGetSubTags);

      ClientServiceProtocol.CreateItemHandler clientCreateItem = new ClientServiceProtocol.CreateItemHandler(CreateItem);
      ClientServiceProtocol.ModifyItemHandler clientModifyItem = new ClientServiceProtocol.ModifyItemHandler(ClientModifyItem);
      ClientServiceProtocol.ModifyItemPathHandler clientModifyItemPath = new ClientServiceProtocol.ModifyItemPathHandler(ClientModifyItemPath);
      ClientServiceProtocol.DeleteItemHandler clientDeleteItem = new ClientServiceProtocol.DeleteItemHandler(ClientDeleteItem);

      ClientServiceProtocol.CreateLinkHandler clientCreateLink = new ClientServiceProtocol.CreateLinkHandler(ClientCreateLink);
      ClientServiceProtocol.ModifyLinkHandler clientModifyLink = new ClientServiceProtocol.ModifyLinkHandler(ClientModifyLink);
      ClientServiceProtocol.DeleteLinkHandler clientDeleteLink = new ClientServiceProtocol.DeleteLinkHandler(ClientDeleteLink);

      ClientServiceProtocol.CreateThingHandler clientCreateThing = new ClientServiceProtocol.CreateThingHandler(ClientCreateThing);
      ClientServiceProtocol.ModifyThingHandler clientModifyThing = new ClientServiceProtocol.ModifyThingHandler(ClientModifyThing);
      ClientServiceProtocol.ModifyThingPathHandler clientModifyThingPath = new ClientServiceProtocol.ModifyThingPathHandler(ClientModifyThingPath);
      ClientServiceProtocol.DeleteThingHandler clientDeleteThing = new ClientServiceProtocol.DeleteThingHandler(ClientDeleteThing);

      ClientServiceProtocol.PortCheckHandler clientPortCheck = new ClientServiceProtocol.PortCheckHandler(ClientPortCheck);

      ClientServiceProtocol.PropertyListHandler clientPropertyListCheck = new ClientServiceProtocol.PropertyListHandler(ClientPropertyListCheck);


      clientClientServiceProtocol = new ClientServiceProtocol(name, 
                                                              graphicLinks, graphicItems, graphicThings,
                                                              clientChangeState, clientGetPropertyValues, clientGetSubTags,
                                                              clientCreateItem, clientModifyItem, clientModifyItemPath, clientDeleteItem,
                                                              clientCreateLink, clientModifyLink, clientDeleteLink,
                                                              clientCreateThing, clientModifyThing, clientModifyThingPath,
                                                              clientDeleteThing, clientPortCheck, clientPropertyListCheck);


      RemotingServices.Marshal(clientClientServiceProtocol, "Client/" + name);


      EngineServiceProtocol.LoadHandler engineLoad = new EngineServiceProtocol.LoadHandler(EngineLoad);
      EngineServiceProtocol.SaveHandler engineSave = new EngineServiceProtocol.SaveHandler(EngineSave);

      EngineServiceProtocol.ChangeStateHandler engineChangeState = new EngineServiceProtocol.ChangeStateHandler(EngineChangeState);

      EngineServiceProtocol.GetPropertyValuesHandler engineGetPropertyValues = new EngineServiceProtocol.GetPropertyValuesHandler(EngineGetPropertyValues);
      EngineServiceProtocol.GetSubTagsHandler engineGetSubTags = new EngineServiceProtocol.GetSubTagsHandler(EngineGetSubTags);

      EngineServiceProtocol.CreateItemHandler engineCreateItem = new EngineServiceProtocol.CreateItemHandler(CreateItem);
      EngineServiceProtocol.ModifyItemHandler engineModifyItem = new EngineServiceProtocol.ModifyItemHandler(EngineModifyItem);
      EngineServiceProtocol.ModifyItemPathHandler engineModifyItemPath = new EngineServiceProtocol.ModifyItemPathHandler(EngineModifyItemPath);
      EngineServiceProtocol.DeleteItemHandler engineDeleteItem = new EngineServiceProtocol.DeleteItemHandler(EngineDeleteItem);

      EngineServiceProtocol.CreateLinkHandler engineCreateLink = new EngineServiceProtocol.CreateLinkHandler(EngineCreateLink);
      EngineServiceProtocol.ModifyLinkHandler engineModifyLink = new EngineServiceProtocol.ModifyLinkHandler(EngineModifyLink);
      EngineServiceProtocol.DeleteLinkHandler engineDeleteLink = new EngineServiceProtocol.DeleteLinkHandler(EngineDeleteLink);

      EngineServiceProtocol.CreateThingHandler engineCreateThing = new EngineServiceProtocol.CreateThingHandler(EngineCreateThing);
      EngineServiceProtocol.ModifyThingHandler engineModifyThing = new EngineServiceProtocol.ModifyThingHandler(EngineModifyThing);
      EngineServiceProtocol.ModifyThingPathHandler engineModifyThingPath = new EngineServiceProtocol.ModifyThingPathHandler(EngineModifyThingPath);
      EngineServiceProtocol.DeleteThingHandler engineDeleteThing = new EngineServiceProtocol.DeleteThingHandler(EngineDeleteThing);

      EngineServiceProtocol.PortCheckHandler enginePortCheck = new EngineServiceProtocol.PortCheckHandler(EnginePortCheck);

      EngineServiceProtocol.PropertyListHandler enginePropertyListCheck = new EngineServiceProtocol.PropertyListHandler(EnginePropertyListCheck);

      engineClientServiceProtocol = new EngineServiceProtocol(name,
                                                              graphicLinks, graphicItems, graphicThings,
                                                              engineLoad, engineSave,
                                                              engineChangeState, engineGetPropertyValues, engineGetSubTags,
                                                              engineCreateItem, engineModifyItem, engineModifyItemPath, engineDeleteItem,
                                                              engineCreateLink, engineModifyLink, engineDeleteLink,
                                                              engineCreateThing, engineModifyThing, engineModifyThingPath,
                                                              engineDeleteThing, enginePortCheck, enginePropertyListCheck);

      EngineLoad();

      RemotingServices.Marshal(engineClientServiceProtocol, "Engine/" + name);

    }



















    bool ClientChangeState(Int64 requestID, BaseProtocol.RunStates runState)
    {
      if (true) // Decide whether to allow runstate change
      { // We're going to do it.
        // Change the runstate.

        // Raise event(s).
        clientClientServiceProtocol.DoStateChanged(requestID, runState);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool CreateItem(Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      // Need to check for runstate here, and decide if we'll fire DoItemCreated.
      // This is required in case a rogue client tries to create an item even when not supposed to.
      // This applies to all three create*, and all three delete* events.
      if (!graphicItems.ContainsKey(guid))
      { // We're going to do it.
        // Create the item.
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = (ARectangleF)boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.FillMode = fillMode;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        // Raise event(s).
        clientClientServiceProtocol.DoItemCreated(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
        engineClientServiceProtocol.DoItemCreated(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientCreateLink(Int64 requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (true) // Decide whether to create an link.
      { // We're going to do it.
        // Create the item.

        // Raise event(s).
        clientClientServiceProtocol.DoLinkCreated(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientCreateThing(Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (true) // Decide whether to create an Thing.
      { // We're going to do it.
        // Create the Thing.

        // Raise event(s).
        clientClientServiceProtocol.DoThingCreated(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientDeleteItem(Int64 requestID, Guid guid)
    {
      if (true) // Decide whether to delete an item.
      { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        clientClientServiceProtocol.DoItemDeleted(requestID, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientDeleteLink(Int64 requestID, Guid guid)
    {
      if (true) // Decide whether to delete an link.
      { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        clientClientServiceProtocol.DoLinkDeleted(requestID, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientDeleteThing(Int64 requestID, Guid guid)
    {
      if (true) // Decide whether to delete an Thing.
      { // We're going to do it.
        // Delete the Thing.

        // Raise event(s).
        clientClientServiceProtocol.DoThingDeleted(requestID, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    void ClientGetPropertyValues(Int64 requestID, ref ArrayList propertyList)
    {
      // Return modified ArrayList with tag details included.
    }

    void ClientGetSubTags(Int64 requestID, String propertyPath, out ArrayList propertyList)
    {
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

    bool ClientModifyItem(Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
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
        clientClientServiceProtocol.DoItemModified(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientModifyItemPath(Int64 requestID, Guid guid, String path)
    {
      if (true) // Decide whether to modify an item.
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        clientClientServiceProtocol.DoItemPathModified(requestID, guid, path);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientModifyLink(Int64 requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (true) // Decide whether to modify an link.
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        clientClientServiceProtocol.DoLinkModified(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientModifyThing(Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (true) // Decide whether to modify an Thing.
      { // We're going to do it.
        // Modify the Thing.

        // Raise event(s).
        clientClientServiceProtocol.DoThingModified(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool ClientModifyThingPath(Int64 requestID, Guid guid, String path)
    {
      if (true) // Decide whether to modify an item.
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        clientClientServiceProtocol.DoThingPathModified(requestID, guid, path);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    PortStatus ClientPortCheck(Guid guid, Anchor anchor)
    {
      //		CNSGuidItem * pGuid = new CNSGuidItem();
      //		pGuid.m_Guid = guid;
      //		m_pUnmanaged.m_Guids.AddTail(null);
      //		CNSGuidItem * pGuid = m_pUnmanaged.m_Guids.Find(guid);
      //		CNSMdlLink * pLink = dynamic_cast<CNSMdlLink *>(guid);
      return PortStatus.Available;
    }

    ArrayList ClientPropertyListCheck(Guid guid, String tag, String path)
    {
      //char* dest = new char[tag.Length+1];
      //strcpy(dest, static_cast<LPCTSTR>(const_cast<void*>(static_cast<const void*>(System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(tag)))));
      //ScdMainWnd().PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)dest);

      ArrayList list = new ArrayList();
      // Generate list of properties at this level in properties tree.
      return list;
    }








    bool ConfirmModelStencil(ModelStencil modelstencil)
    {
      // TODO: check whether this stencil is to be included in the project.
      return true;
    }

    bool EngineChangeState(Int64 requestID, BaseProtocol.RunStates runState)
    {
      if (true) // Decide whether to allow runstate change
      { // We're going to do it.
        // Change the runstate.

        // Raise event(s).
        engineClientServiceProtocol.DoStateChanged(requestID, runState);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineCreateLink(Int64 requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (true) // Decide whether to create an link.
      { // We're going to do it.
        // Create the item.

        // Raise event(s).
        engineClientServiceProtocol.DoLinkCreated(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineCreateThing(Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (true) // Decide whether to create an Thing.
      { // We're going to do it.
        // Create the Thing.

        // Raise event(s).
        engineClientServiceProtocol.DoThingCreated(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineDeleteItem(Int64 requestID, Guid guid)
    {
      if (true) // Decide whether to delete an item.
      { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        engineClientServiceProtocol.DoItemDeleted(requestID, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineDeleteLink(Int64 requestID, Guid guid)
    {
      if (true) // Decide whether to delete an link.
      { // We're going to do it.
        // Delete the item.

        // Raise event(s).
        engineClientServiceProtocol.DoLinkDeleted(requestID, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineDeleteThing(Int64 requestID, Guid guid)
    {
      if (true) // Decide whether to delete an Thing.
      { // We're going to do it.
        // Delete the Thing.

        // Raise event(s).
        engineClientServiceProtocol.DoThingDeleted(requestID, guid);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    void EngineGetPropertyValues(Int64 requestID, ref ArrayList propertyList)
    {
      // Return modified ArrayList with tag details included.
    }

    void EngineGetSubTags(Int64 requestID, String propertyPath, out ArrayList propertyList)
    {
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





    bool EngineLoad()
    {
      try
      {
        Dictionary<Guid, GraphicLink> tempGraphicLinks;
        Dictionary<Guid, GraphicItem> tempGraphicItems;
        Dictionary<Guid, GraphicThing> tempGraphicThings;

        {
          SoapFormatter sf = new SoapFormatter();
          StreamReader streamRdr = new StreamReader(projectPath + "GraphicLinks.10");
          Stream stream = streamRdr.BaseStream;
          tempGraphicLinks = (Dictionary<Guid, GraphicLink>)sf.Deserialize(stream);
          stream.Close();
        }

        {
          SoapFormatter sf = new SoapFormatter();
          StreamReader streamRdr = new StreamReader(projectPath + "GraphicItems.10");
          Stream stream = streamRdr.BaseStream;
          tempGraphicItems = (Dictionary<Guid, GraphicItem>)sf.Deserialize(stream);
          stream.Close();
        }

        {
          SoapFormatter sf = new SoapFormatter();
          StreamReader streamRdr = new StreamReader(projectPath + "GraphicThings.10");
          Stream stream = streamRdr.BaseStream;
          tempGraphicThings = (Dictionary<Guid, GraphicThing>)sf.Deserialize(stream);
          stream.Close();
        }

        graphicLinks = tempGraphicLinks;
        graphicItems = tempGraphicItems;
        graphicThings = tempGraphicThings;

        if (graphicLinks == null)
          graphicLinks = new Dictionary<Guid,GraphicLink>();
        if (graphicItems == null)
          graphicItems = new Dictionary<Guid,GraphicItem>();
        if (graphicThings == null)
          graphicThings = new Dictionary<Guid,GraphicThing>();

        return true;
      }
      catch (Exception)
      {
        if (graphicLinks == null)
          graphicLinks = new Dictionary<Guid, GraphicLink>();
        if (graphicItems == null)
          graphicItems = new Dictionary<Guid, GraphicItem>();
        if (graphicThings == null)
          graphicThings = new Dictionary<Guid, GraphicThing>();

        return false;
      }
    }

    bool EngineModifyItem(Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
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
        engineClientServiceProtocol.DoItemModified(requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineModifyItemPath(Int64 requestID, Guid guid, String path)
    {
      if (true) // Decide whether to modify an item.
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        engineClientServiceProtocol.DoItemPathModified(requestID, guid, path);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineModifyLink(Int64 requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (true) // Decide whether to modify an link.
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        engineClientServiceProtocol.DoLinkModified(requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineModifyThing(Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      if (true) // Decide whether to modify an Thing.
      { // We're going to do it.
        // Modify the Thing.

        // Raise event(s).
        engineClientServiceProtocol.DoThingModified(requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    bool EngineModifyThingPath(Int64 requestID, Guid guid, String path)
    {
      if (true) // Decide whether to modify an item.
      { // We're going to do it.
        // Modify the item.

        // Raise event(s).
        engineClientServiceProtocol.DoThingPathModified(requestID, guid, path);

        return true;
      }
      else
      { // We're not going to do it.
        return false;
      }
    }

    PortStatus EnginePortCheck(Guid guid, Anchor anchor)
    {
      //		CNSGuidItem * pGuid = new CNSGuidItem();
      //		pGuid.m_Guid = guid;
      //		m_pUnmanaged.m_Guids.AddTail(null);
      //		CNSGuidItem * pGuid = m_pUnmanaged.m_Guids.Find(guid);
      //		CNSMdlLink * pLink = dynamic_cast<CNSMdlLink *>(guid);
      return PortStatus.Available;
    }

    ArrayList EnginePropertyListCheck(Guid guid, String tag, String path)
    {
      //char* dest = new char[tag.Length+1];
      //strcpy(dest, static_cast<LPCTSTR>(const_cast<void*>(static_cast<const void*>(System.Runtime.InteropServices.Marshal.StringToHGlobalAnsi(tag)))));
      //ScdMainWnd().PostMessage(WMU_TAGACTION, SUB_TAGACTION_FINDANDACCESS, (LPARAM)dest);

      ArrayList list = new ArrayList();
      // Generate list of properties at this level in properties tree.
      return list;
    }

    bool EngineSave()
    {
      try
      {
        {
          SoapFormatter sf = new SoapFormatter();
          StreamWriter streamWriter = new StreamWriter(projectPath + "GraphicLinks.10");
          Stream stream = streamWriter.BaseStream;
          sf.Serialize(stream, graphicLinks);
          stream.Close();
        }

        {
          SoapFormatter sf = new SoapFormatter();
          StreamWriter streamWriter = new StreamWriter(projectPath + "GraphicItems.10");
          Stream stream = streamWriter.BaseStream;
          sf.Serialize(stream, graphicItems);
          stream.Close();
        }

        {
          SoapFormatter sf = new SoapFormatter();
          StreamWriter streamWriter = new StreamWriter(projectPath + "GraphicThings.10");
          Stream stream = streamWriter.BaseStream;
          sf.Serialize(stream, graphicThings);
          stream.Close();
        }
        return true;
      }
      catch (Exception)
      {
        return false;
      }
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
  }
}
