
using System;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using System.Runtime.Remoting.Lifetime;
using System.Drawing;
using System.Threading;

using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting.Channels;
using System.Collections;
using System.Runtime.Remoting.Channels.Tcp;

//using System.Security.Permissions;
using System.Drawing.Drawing2D;

namespace SysCAD.Protocol
{

  [Serializable]
  public sealed class ClientProtocol : ClientBaseProtocol
  {

    public String connectionError = String.Empty;
    public String clientName = String.Empty;

    private ClientServiceProtocol serviceGraphic;

    private ClientServiceProtocol.GroupCreatedHandler serviceGraphicGroupCreatedHandler = null;
    private ClientServiceProtocol.GroupDeletedHandler serviceGraphicGroupDeletedHandler = null;
    private ClientServiceProtocol.GroupModifiedHandler serviceGraphicGroupModifiedHandler = null;

    private ClientServiceProtocol.ItemCreatedHandler serviceGraphicItemCreatedHandler = null;
    private ClientServiceProtocol.ItemDeletedHandler serviceGraphicItemDeletedHandler = null;
    private ClientServiceProtocol.PortInfoRequestedHandler serviceGraphicPortInfoRequestedHandler = null;
    private ClientServiceProtocol.ItemModifiedHandler serviceGraphicItemModifiedHandler = null;

    private ClientServiceProtocol.LinkCreatedHandler serviceGraphicLinkCreatedHandler = null;
    private ClientServiceProtocol.LinkDeletedHandler serviceGraphicLinkDeletedHandler = null;
    private ClientServiceProtocol.LinkModifiedHandler serviceGraphicLinkModifiedHandler = null;

    private ClientServiceProtocol.PermissionsChangedHandler serviceGraphicPermissionsChangedHandler = null;

    private ClientServiceProtocol.StepHandler serviceGraphicStepHandler = null;

    private ClientServiceProtocol.SyncHandler serviceGraphicSyncHandler = null;

    private ClientServiceProtocol.ThingCreatedHandler serviceGraphicThingCreatedHandler = null;
    private ClientServiceProtocol.ThingDeletedHandler serviceGraphicThingDeletedHandler = null;
    private ClientServiceProtocol.ThingModifiedHandler serviceGraphicThingModifiedHandler = null;

    Uri url = null;

    public ClientProtocol()
    {
    }

    //[EnvironmentPermissionAttribute(SecurityAction.LinkDemand, Unrestricted = true)]
    public bool Connect(String clientName)
    {

      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as ClientServiceProtocol;

        Name = serviceGraphic.Name; // Force a test of the connection.

        serviceGraphicPermissionsChangedHandler = new ClientServiceProtocol.PermissionsChangedHandler(ServiceGraphicPermissionsChanged);

        serviceGraphicStepHandler = new ClientServiceProtocol.StepHandler(ServiceGraphicStep);

        serviceGraphicSyncHandler = new ClientServiceProtocol.SyncHandler(ServiceGraphicSync);

        serviceGraphicGroupCreatedHandler = new ClientServiceProtocol.GroupCreatedHandler(ServiceGraphicGroupCreated);
        serviceGraphicGroupModifiedHandler = new ClientServiceProtocol.GroupModifiedHandler(ServiceGraphicGroupModified);
        serviceGraphicGroupDeletedHandler = new ClientServiceProtocol.GroupDeletedHandler(ServiceGraphicGroupDeleted);

        serviceGraphicItemCreatedHandler = new ClientServiceProtocol.ItemCreatedHandler(ServiceGraphicItemCreated);
        serviceGraphicPortInfoRequestedHandler = new ClientServiceProtocol.PortInfoRequestedHandler(ServiceGraphicPortInfoRequested);
        serviceGraphicItemModifiedHandler = new ClientServiceProtocol.ItemModifiedHandler(ServiceGraphicItemModified);
        serviceGraphicItemDeletedHandler = new ClientServiceProtocol.ItemDeletedHandler(ServiceGraphicItemDeleted);

        serviceGraphicLinkCreatedHandler = new ClientServiceProtocol.LinkCreatedHandler(ServiceGraphicLinkCreated);
        serviceGraphicLinkModifiedHandler = new ClientServiceProtocol.LinkModifiedHandler(ServiceGraphicLinkModified);
        serviceGraphicLinkDeletedHandler = new ClientServiceProtocol.LinkDeletedHandler(ServiceGraphicLinkDeleted);

        serviceGraphicThingCreatedHandler = new ClientServiceProtocol.ThingCreatedHandler(ServiceGraphicThingCreated);
        serviceGraphicThingModifiedHandler = new ClientServiceProtocol.ThingModifiedHandler(ServiceGraphicThingModified);
        serviceGraphicThingDeletedHandler = new ClientServiceProtocol.ThingDeletedHandler(ServiceGraphicThingDeleted);

        serviceGraphic.PermissionsChanged += serviceGraphicPermissionsChangedHandler;

        serviceGraphic.Step += serviceGraphicStepHandler;

        serviceGraphic.Sync += serviceGraphicSyncHandler;

        serviceGraphic.GroupCreated += serviceGraphicGroupCreatedHandler;
        serviceGraphic.GroupModified += serviceGraphicGroupModifiedHandler;
        serviceGraphic.GroupDeleted += serviceGraphicGroupDeletedHandler;

        serviceGraphic.ItemCreated += serviceGraphicItemCreatedHandler;
        serviceGraphic.PortInfoRequested += serviceGraphicPortInfoRequestedHandler;
        serviceGraphic.ItemModified += serviceGraphicItemModifiedHandler;
        serviceGraphic.ItemDeleted += serviceGraphicItemDeletedHandler;

        serviceGraphic.LinkCreated += serviceGraphicLinkCreatedHandler;
        serviceGraphic.LinkModified += serviceGraphicLinkModifiedHandler;
        serviceGraphic.LinkDeleted += serviceGraphicLinkDeletedHandler;

        serviceGraphic.ThingCreated += serviceGraphicThingCreatedHandler;
        serviceGraphic.ThingModified += serviceGraphicThingModifiedHandler;
        serviceGraphic.ThingDeleted += serviceGraphicThingDeletedHandler;

        Syncxxx();

        this.clientName = clientName;
        Announce(ref this.clientName);

        connectionError = "";
          return true;
      }

      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        connectionError = remotingException.Message;
        return false;
      }
    }

    private void Announce(ref string name)
    {
      serviceGraphic.Announce(ref name);
    }

    private void Renounce(string name)
    {
      serviceGraphic.Renounce(name);
    }

    public bool CreateGroup(out Int64 requestId, out Guid guid, String tag, String path, Rectangle boundingRect)
    {
      return serviceGraphic.CreateGroup(out requestId, out guid, tag, path, boundingRect);
    }

    public bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.CreateItem(out requestId, out guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool CreateLink(out Int64 requestId, out Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      return serviceGraphic.CreateLink(out requestId, out guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints, textArea, textAngle);
    }

    public bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.CreateThing(out requestId, out guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool DeleteGroup(out Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public bool DeleteItem(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteItem(out requestId, guid);
    }

    public bool DeleteLink(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteLink(out requestId, guid);
    }

    public bool DeleteThing(out Int64 requestId, Guid guid)
    {
      return serviceGraphic.DeleteThing(out requestId, guid);
    }

    public void GetPropertyValues(out Int64 requestId, ref ArrayList tagPathList)
    {
      serviceGraphic.GetPropertyValues(out requestId, ref tagPathList);
    }

    public void GetSubTags(out Int64 requestId, String propertyPath, out ArrayList propertyList)
    {
      serviceGraphic.GetSubTags(out requestId, propertyPath, out propertyList);
    }

    public bool Load(out Int64 requestId)
    {
      return serviceGraphic.Load(out requestId);
    }

    public bool ModifyGroup(out Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public bool RequestPortInfo(out Int64 requestId, Guid guid, String tag)
    {
      return serviceGraphic.RequestPortInfo(out requestId, guid, tag);
    }

    public bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyItem(out requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyItemPath(out Int64 requestId, Guid guid, String path)
    {
      return serviceGraphic.ModifyItemPath(out requestId, guid, path);
    }

    public bool ModifyItemBoundingRect(out Int64 requestId, Guid guid, Rectangle boundingRect)
    {
      return serviceGraphic.ModifyItemBoundingRect(out requestId, guid, boundingRect);
    }

    public bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      return serviceGraphic.ModifyLink(out requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints, textArea, textAngle);
    }

    public bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      return serviceGraphic.ModifyThing(out requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool ModifyThingPath(out Int64 requestId, Guid guid, String path)
    {
      return serviceGraphic.ModifyThingPath(out requestId, guid, path);
    }

    public ArrayList PropertyList(out Int64 requestId, Guid guid, String tag, String path)
    {
      return serviceGraphic.PropertyList(out requestId, guid, tag, path);
    }

    public void LogMessage(out Int64 requestId, String message, SysCAD.Log.MessageType messageType)
    {
      serviceGraphic.LogMessage(out requestId, message, messageType);
    }

    public bool Save(out Int64 requestId)
    {
      return serviceGraphic.Save(out requestId);
    }

    public void ServiceGraphicGroupCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    {
      if (!graphicGroups.ContainsKey(guid))
      {
        GraphicGroup graphicGroup = new GraphicGroup(guid, tag);
        graphicGroup.Path = path;
        graphicGroup.BoundingRect = boundingRect;
        
        graphicGroups.Add(guid, graphicGroup);

        OnGroupCreated(eventId, requestId, guid, tag, path, boundingRect);
      }
    }

    public void ServiceGraphicGroupDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void ServiceGraphicGroupModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    public void ServiceGraphicItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {

      if (!graphicItems.ContainsKey(guid))
      {
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.TextArea = textArea;
        graphicItem.TextAngle = textAngle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicItems.ContainsKey(guid))
      {
        graphicItems.Remove(guid);

        OnItemDeleted(eventId, requestId, guid);
      }
    }

    public void ServiceGraphicPortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      OnPortInfoRequested(eventId, requestId, guid, tag, portInfo);
    }

    public void ServiceGraphicItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, Rectangle boundingRect, Double angle, Rectangle textArea, Double textAngle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      GraphicItem graphicItem;

      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Tag = tag;
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.TextArea = textArea;
        graphicItem.TextAngle = textAngle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, textArea, textAngle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {

      if (!graphicLinks.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag, classId, origin, originPort, destination, destinationPort, controlPoints, textArea, textAngle);
        graphicLinks.Add(guid, graphicLink);

        OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints, textArea, textAngle);
      }
    }

    public void ServiceGraphicLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        OnLinkDeleted(eventId, requestId, guid);
      }
    }

    public void ServiceGraphicLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      GraphicLink graphicLink;

      if (graphicLinks.TryGetValue(guid, out graphicLink))
      {
        graphicLink.Tag = tag;
        graphicLink.ClassID = classId;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;

        graphicLink.ControlPoints.Clear();

        foreach (Point controlPoint in controlPoints)
          graphicLink.ControlPoints.Add(controlPoint);

        graphicLink.TextArea = textArea;
        graphicLink.TextAngle = textAngle;

        OnLinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints, textArea, textAngle);
      }
    }

    public void ServiceGraphicPermissionsChanged(Int64 eventId, Int64 requestId, Permissions permissions)
    {
      OnPermissionsChanged(eventId, requestId, permissions);
    }

    public void ServiceGraphicStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void ServiceGraphicSync(Int64 eventId)
    {
      OnSync(eventId);
    }

    public void ServiceGraphicThingCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {

      if (!graphicThings.ContainsKey(guid))
      {
        GraphicThing graphicThing = new GraphicThing(guid, tag);
        graphicThing.Path = path;
        graphicThing.BoundingRect = boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        graphicThings.Add(guid, graphicThing);

        OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {

      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        OnThingDeleted(eventId, requestId, guid);
      }
    }

    public void ServiceGraphicThingModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Rectangle boundingRect, String xaml, Double angle, bool mirrorX, bool mirrorY)
    {
      GraphicThing graphicThing;

      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Tag = tag;
        graphicThing.Path = path;
        graphicThing.BoundingRect = boundingRect;
        graphicThing.Xaml = xaml;
        graphicThing.Angle = angle;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        OnThingModified(eventId, requestId, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void Syncxxx()
    {
      MemoryStream memoryStream;
      BinaryFormatter bf = new BinaryFormatter();

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicGroups);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicGroups = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicGroup>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicLinks);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicLinks = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicLink>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicItems);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicItems = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicItem>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, serviceGraphic.graphicThings);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicThings = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicThing>;
    }

    public bool TestUrl(Uri url)
    {

      try
      {
        serviceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as ClientServiceProtocol;

        Name = serviceGraphic.Name; // Force a test of the connection.

        this.url = url;
        connectionError = "";
        return true;
      }

      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        url = null;
        connectionError = remotingException.Message;
        return false;
      }
    }

    ~ClientProtocol()
    {
      Renounce(clientName);

      if (serviceGraphic != null)
      {

        try
        {

          if (serviceGraphicPermissionsChangedHandler != null) serviceGraphic.PermissionsChanged -= serviceGraphicPermissionsChangedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicStepHandler != null) serviceGraphic.Step -= serviceGraphicStepHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicSyncHandler != null) serviceGraphic.Sync -= serviceGraphicSyncHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicGroupCreatedHandler != null) serviceGraphic.GroupCreated -= serviceGraphicGroupCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicGroupModifiedHandler != null) serviceGraphic.GroupModified -= serviceGraphicGroupModifiedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicGroupDeletedHandler != null) serviceGraphic.GroupDeleted -= serviceGraphicGroupDeletedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicItemCreatedHandler != null) serviceGraphic.ItemCreated -= serviceGraphicItemCreatedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicPortInfoRequestedHandler != null) serviceGraphic.PortInfoRequested -= serviceGraphicPortInfoRequestedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicItemModifiedHandler != null) serviceGraphic.ItemModified -= serviceGraphicItemModifiedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicItemDeletedHandler != null) serviceGraphic.ItemDeleted -= serviceGraphicItemDeletedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicLinkCreatedHandler != null) serviceGraphic.LinkCreated -= serviceGraphicLinkCreatedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicLinkModifiedHandler != null) serviceGraphic.LinkModified -= serviceGraphicLinkModifiedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicLinkDeletedHandler != null) serviceGraphic.LinkDeleted -= serviceGraphicLinkDeletedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {
          if (serviceGraphicThingCreatedHandler != null) serviceGraphic.ThingCreated -= serviceGraphicThingCreatedHandler;
        }
        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingModifiedHandler != null) serviceGraphic.ThingModified -= serviceGraphicThingModifiedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingDeletedHandler != null) serviceGraphic.ThingDeleted -= serviceGraphicThingDeletedHandler;
        }

        catch (InvalidOperationException) { }
      }
    }
  }
}
