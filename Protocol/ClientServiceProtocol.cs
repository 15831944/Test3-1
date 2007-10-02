
using System;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using System.Runtime.Remoting.Lifetime;
using System.Drawing;
using System.Threading;

//using System.Runtime.Remoting.Channels.Ipc;
//using System.Runtime.Remoting.Channels;
using System.Collections;

//using System.Runtime.Remoting.Channels.Tcp;
using System.Drawing.Drawing2D;

namespace SysCAD.Protocol
{

  [Serializable]
  public sealed class ClientServiceProtocol : ClientBaseProtocol
  {
    private LoadHandler loadHandler;
    private SaveHandler saveHandler;

    private ChangePermissionsHandler clientChangePermissions;

    private CreateGroupHandler createGroupHandler;
    private CreateItemHandler createItemHandler;

    private CreateLinkHandler createLinkHandler;

    private CreateThingHandler createThingHandler;
    private DeleteGroupHandler deleteGroupHandler;
    private DeleteItemHandler deleteItemHandler;
    private DeleteLinkHandler deleteLinkHandler;
    private DeleteThingHandler deleteThingHandler;

    private GetPropertyValuesHandler getPropertyValuesHandler;
    private GetSubTagsHandler getSubTagsHandler;
    private ModifyGroupHandler modifyGroupHandler;
    private RequestPortInfoHandler requestPortInfoHandler;
    private ModifyItemHandler modifyItemHandler;
    private ModifyItemPathHandler modifyItemPathHandler;
    private ModifyItemBoundingRectHandler modifyItemBoundingRectHandler;
    private ModifyLinkHandler modifyLinkHandler;
    private ModifyThingHandler modifyThingHandler;
    private ModifyThingPathHandler modifyThingPathHandler;

    private PropertyListHandler propertyListHandler;

    private LogMessageHandler logMessageHandler;

    private AnnounceHandler announceHandler;
    private RenounceHandler renounceHandler;

    public delegate bool LoadHandler(out Int64 requestId);
    public delegate bool SaveHandler(out Int64 requestId);

    public delegate bool ChangePermissionsHandler(out Int64 requestId, Permissions permissions);

    public delegate bool CreateGroupHandler(out Int64 requestId, out Guid guid, String tag, String path, Rectangle boundingRect);
    public delegate bool CreateItemHandler(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool CreateLinkHandler(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<Point> controlPoints, Rectangle textArea, Double textAngle);
    public delegate bool CreateThingHandler(out Int64 requestId, out Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY);

    public delegate bool DeleteGroupHandler(out Int64 requestId, Guid guid);
    public delegate bool DeleteItemHandler(out Int64 requestId, Guid guid);
    public delegate bool DeleteLinkHandler(out Int64 requestId, Guid guid);
    public delegate bool DeleteThingHandler(out Int64 requestId, Guid guid);

    public delegate void GetPropertyValuesHandler(out Int64 requestId, ref ArrayList propertyList);
    public delegate void GetSubTagsHandler(out Int64 requestId, String propertyPath, out ArrayList propertyList);

    public delegate bool ModifyGroupHandler(out Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect);
    public delegate bool RequestPortInfoHandler(out Int64 requestId, Guid guid, String tag);
    public delegate bool ModifyItemHandler(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemPathHandler(out Int64 requestId, Guid guid, String path);
    public delegate bool ModifyItemBoundingRectHandler(out Int64 requestId, Guid guid, Rectangle boundingRect);

    public delegate bool ModifyLinkHandler(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination,
      String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<Point> controlPoints, Rectangle textArea, Double textAngle);

    public delegate bool ModifyThingHandler(out Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY);
    public delegate bool ModifyThingPathHandler(out Int64 requestId, Guid guid, String path);

    public delegate PortStatus PortCheckHandler(out Int64 requestId, Guid itemGuid, String tag);
    public delegate ArrayList PropertyListHandler(out Int64 requestId, Guid guid, String tag, String path);

    public delegate void LogMessageHandler(out Int64 requestId, String message, SysCAD.Log.MessageType messageType);

    public delegate String AnnounceHandler(String clientName);
    public delegate void RenounceHandler(String clientName);

    public ClientServiceProtocol(String name,
      LoadHandler loadHandler, SaveHandler saveHandler, 
      Dictionary<Guid, GraphicGroup> graphicGroups, Dictionary<Guid, GraphicLink> graphicLinks, 
      Dictionary<Guid, GraphicItem> graphicItems, Dictionary<Guid, GraphicThing> graphicThings,
      ChangePermissionsHandler clientChangePermissions, GetPropertyValuesHandler getPropertyValuesHandler, 
      GetSubTagsHandler getSubTagsHandler,
      CreateGroupHandler createGroupHandler, ModifyGroupHandler modifyGroupHandler, DeleteGroupHandler deleteGroupHandler,
      CreateItemHandler createItemHandler, RequestPortInfoHandler requestPortInfoHandler,
      ModifyItemHandler modifyItemHandler, ModifyItemPathHandler modifyItemPathHandler, ModifyItemBoundingRectHandler modifyItemBoundingRectHandler, 
      DeleteItemHandler deleteItemHandler,
      CreateLinkHandler createLinkHandler, ModifyLinkHandler modifyLinkHandler, DeleteLinkHandler deleteLinkHandler,
      CreateThingHandler createThingHandler, ModifyThingHandler modifyThingHandler, ModifyThingPathHandler modifyThingPathHandler, DeleteThingHandler deleteThingHandler,
      PropertyListHandler propertyListHandler, LogMessageHandler logMessageHandler,
      AnnounceHandler announceHandler, RenounceHandler renounceHandler)
    {
      this.Name = name;

      this.graphicGroups = graphicGroups;

      this.graphicLinks = graphicLinks;
      this.graphicItems = graphicItems;
      this.graphicThings = graphicThings;

      this.loadHandler = loadHandler;
      this.saveHandler = saveHandler;

      this.clientChangePermissions = clientChangePermissions;

      this.getPropertyValuesHandler = getPropertyValuesHandler;
      this.getSubTagsHandler = getSubTagsHandler;

      this.createGroupHandler = createGroupHandler;
      this.modifyGroupHandler = modifyGroupHandler;
      this.deleteGroupHandler = deleteGroupHandler;

      this.createItemHandler = createItemHandler;
      this.requestPortInfoHandler = requestPortInfoHandler;
      this.modifyItemHandler = modifyItemHandler;
      this.modifyItemPathHandler = modifyItemPathHandler;
      this.modifyItemBoundingRectHandler = modifyItemBoundingRectHandler;
      this.deleteItemHandler = deleteItemHandler;

      this.createLinkHandler = createLinkHandler;
      this.modifyLinkHandler = modifyLinkHandler;
      this.deleteLinkHandler = deleteLinkHandler;

      this.createThingHandler = createThingHandler;
      this.modifyThingHandler = modifyThingHandler;
      this.modifyThingPathHandler = modifyThingPathHandler;
      this.deleteThingHandler = deleteThingHandler;

      this.propertyListHandler = propertyListHandler;

      this.logMessageHandler = logMessageHandler;

      this.announceHandler = announceHandler;
      this.renounceHandler = renounceHandler;
    }

    public bool ChangePermissions(out Int64 requestId, Permissions permissions)
    {
      return clientChangePermissions(out requestId, permissions);
    }

    public bool CreateGroup(out Int64 requestId, out Guid guid, String tag, String path, Rectangle boundingRect)
    {
      return createGroupHandler(out requestId, out guid, tag, path, boundingRect);
    }

    public bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return createItemHandler(out requestId, out guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool CreateLink(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      return createLinkHandler(out requestId, out guid, tag, classId, origin, destination, originPort, originPortID, destinationPort, destinationPortID, controlPoints, textArea, textAngle);
    }

    public bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      return createThingHandler(out requestId, out guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool DeleteGroup(out Int64 requestId, Guid guid)
    {
      return deleteGroupHandler(out requestId, guid);
    }

    public bool DeleteItem(out Int64 requestId, Guid guid)
    {
      return deleteItemHandler(out requestId, guid);
    }

    public bool DeleteLink(out Int64 requestId, Guid guid)
    {
      return deleteLinkHandler(out requestId, guid);
    }

    public bool DeleteThing(out Int64 requestId, Guid guid)
    {
      return deleteThingHandler(out requestId, guid);
    }

    public void DoGroupCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    {
      OnGroupCreated(eventId, requestId, guid, tag, path, boundingRect);
    }

    public void DoGroupDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void DoGroupModified(Int64 eventId, Int64 requestId, Guid guid, String tag)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void DoItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, mirrorX, mirrorY);
    }

    public void DoItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      OnItemDeleted(eventId, requestId, guid);
    }

    public void DoPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      OnPortInfoRequested(eventId, requestId, guid, tag, portInfo);
    }

    public void DoItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, mirrorX, mirrorY);
    }

    public void DoLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, originPortID, destinationPort, destinationPortID, controlPoints, textArea, textAngle);
    }

    public void DoLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      OnLinkDeleted(eventId, requestId, guid);
    }

    public void DoLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      OnLinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints, textArea, textAngle);
    }

    public void DoPermissionsChanged(Int64 eventId, Int64 requestId, Permissions permissions)
    {
      OnPermissionsChanged(eventId, requestId, permissions);
    }

    public void DoStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void DoSync(Int64 eventId)
    {
      OnSync(eventId);
    }

    public void DoThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public void DoThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        OnThingDeleted(eventId, requestId, guid);
      }
    }

    public void DoThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      OnThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      getPropertyValuesHandler(out requestId, ref tagPathList);
    }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      getSubTagsHandler(out requestId, propertyPath, out propertyList);
    }

    public bool Load(out Int64 requestId)
    {
      return loadHandler(out requestId);
    }

    public bool ModifyGroup(out Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    {
      return modifyGroupHandler(out requestId, guid, tag, path, boundingRect);
    }

    public bool RequestPortInfo(out Int64 requestId, Guid guid, String tag)
    {
      return requestPortInfoHandler(out requestId, guid, tag);
    }

    public bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return modifyItemHandler(out requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyItemPath(out Int64 requestId, Guid guid, String path)
    {
      return modifyItemPathHandler(out requestId, guid, path);
    }

    public bool ModifyItemBoundingRect(out Int64 requestId, Guid guid, Rectangle boundingRect)
    {
      return modifyItemBoundingRectHandler(out requestId, guid, boundingRect);
    }

    public bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination,
      String originPort, Int16 originPortID, String destinationPort, Int16 destinationPortID, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      return modifyLinkHandler(out requestId, guid, tag, classId, origin, destination,
        originPort, originPortID, destinationPort, destinationPortID, controlPoints, textArea, textAngle);
    }

    public bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      return modifyThingHandler(out requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool ModifyThingPath(out Int64 requestId, Guid guid, String path)
    {
      return modifyThingPathHandler(out requestId, guid, path);
    }

    public ArrayList PropertyList(out Int64 requestId, Guid guid, String tag, String path)
    {
      //todo: check path is valid.
      return propertyListHandler(out requestId, guid, tag, path);
    }

    public bool Save(out Int64 requestId)
    {
      return saveHandler(out requestId);
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      logMessageHandler(out requestId, message, messageType);
    }

    public string Announce(string clientName)
    {
      return announceHandler(clientName);
    }

    public void Renounce(string clientName)
    {
      renounceHandler(clientName);
    }
  }
}
