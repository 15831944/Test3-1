
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
  public sealed class ClientProtocol : BaseProtocol
  {

    public String connectionError = String.Empty;

    private ClientServiceProtocol clientServiceGraphic;

    private ClientServiceProtocol.ItemCreatedHandler serviceGraphicItemCreatedHandler = null;
    private ClientServiceProtocol.ItemDeletedHandler serviceGraphicItemDeletedHandler = null;
    private ClientServiceProtocol.ItemModifiedHandler serviceGraphicItemModifiedHandler = null;

    private ClientServiceProtocol.LinkCreatedHandler serviceGraphicLinkCreatedHandler = null;
    private ClientServiceProtocol.LinkDeletedHandler serviceGraphicLinkDeletedHandler = null;
    private ClientServiceProtocol.LinkModifiedHandler serviceGraphicLinkModifiedHandler = null;

    private ClientServiceProtocol.StateChangedHandler serviceGraphicStateChangedHandler = null;

    private ClientServiceProtocol.StepHandler serviceGraphicStepHandler = null;

    private ClientServiceProtocol.SyncHandler serviceGraphicSyncHandler = null;

    private ClientServiceProtocol.ThingCreatedHandler serviceGraphicThingCreatedHandler = null;
    private ClientServiceProtocol.ThingDeletedHandler serviceGraphicThingDeletedHandler = null;
    private ClientServiceProtocol.ThingModifiedHandler serviceGraphicThingModifiedHandler = null;

    Uri url = null;

    public ClientProtocol()
    {
    }

    public bool ChangeState(out Int64 requestID, RunStates runState)
    {
      return clientServiceGraphic.ChangeState(out requestID, runState);
    }

    //[EnvironmentPermissionAttribute(SecurityAction.LinkDemand, Unrestricted = true)]
    public bool Connect()
    {

      try
      {
        clientServiceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as ClientServiceProtocol;

        Name = clientServiceGraphic.Name; // Force a test of the connection.

        serviceGraphicStateChangedHandler = new ClientServiceProtocol.StateChangedHandler(ServiceGraphicStateChanged);

        serviceGraphicStepHandler = new ClientServiceProtocol.StepHandler(ServiceGraphicStep);

        serviceGraphicSyncHandler = new ClientServiceProtocol.SyncHandler(ServiceGraphicSync);

        serviceGraphicItemCreatedHandler = new ClientServiceProtocol.ItemCreatedHandler(ServiceGraphicItemCreated);
        serviceGraphicItemModifiedHandler = new ClientServiceProtocol.ItemModifiedHandler(ServiceGraphicItemModified);
        serviceGraphicItemDeletedHandler = new ClientServiceProtocol.ItemDeletedHandler(ServiceGraphicItemDeleted);

        serviceGraphicLinkCreatedHandler = new ClientServiceProtocol.LinkCreatedHandler(ServiceGraphicLinkCreated);
        serviceGraphicLinkModifiedHandler = new ClientServiceProtocol.LinkModifiedHandler(ServiceGraphicLinkModified);
        serviceGraphicLinkDeletedHandler = new ClientServiceProtocol.LinkDeletedHandler(ServiceGraphicLinkDeleted);

        serviceGraphicThingCreatedHandler = new ClientServiceProtocol.ThingCreatedHandler(ServiceGraphicThingCreated);
        serviceGraphicThingModifiedHandler = new ClientServiceProtocol.ThingModifiedHandler(ServiceGraphicThingModified);
        serviceGraphicThingDeletedHandler = new ClientServiceProtocol.ThingDeletedHandler(ServiceGraphicThingDeleted);

        clientServiceGraphic.StateChanged += serviceGraphicStateChangedHandler;

        clientServiceGraphic.Step += serviceGraphicStepHandler;

        clientServiceGraphic.Sync += serviceGraphicSyncHandler;

        clientServiceGraphic.ItemCreated += serviceGraphicItemCreatedHandler;
        clientServiceGraphic.ItemModified += serviceGraphicItemModifiedHandler;
        clientServiceGraphic.ItemDeleted += serviceGraphicItemDeletedHandler;

        clientServiceGraphic.LinkCreated += serviceGraphicLinkCreatedHandler;
        clientServiceGraphic.LinkModified += serviceGraphicLinkModifiedHandler;
        clientServiceGraphic.LinkDeleted += serviceGraphicLinkDeletedHandler;

        clientServiceGraphic.ThingCreated += serviceGraphicThingCreatedHandler;
        clientServiceGraphic.ThingModified += serviceGraphicThingModifiedHandler;
        clientServiceGraphic.ThingDeleted += serviceGraphicThingDeletedHandler;

        Syncxxx();

        connectionError = "";
        return true;
      }

      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        connectionError = remotingException.Message;
        return false;
      }
    }

    public bool CreateItem(out Int64 requestID, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return clientServiceGraphic.CreateItem(out requestID, out guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool CreateLink(out Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return clientServiceGraphic.CreateLink(out requestID, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool CreateThing(out Int64 requestID, out Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return clientServiceGraphic.CreateThing(out requestID, out guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool DeleteItem(out Int64 requestID, Guid guid)
    {
      return clientServiceGraphic.DeleteItem(out requestID, guid);
    }

    public bool DeleteLink(out Int64 requestID, Guid guid)
    {
      return clientServiceGraphic.DeleteLink(out requestID, guid);
    }

    public bool DeleteThing(out Int64 requestID, Guid guid)
    {
      return clientServiceGraphic.DeleteThing(out requestID, guid);
    }

    public void GetPropertyValues(out Int64 requestID, ref ArrayList tagPathList)
    {
      clientServiceGraphic.GetPropertyValues(out requestID, ref tagPathList);
    }

    public void GetSubTags(out Int64 requestID, String propertyPath, out ArrayList propertyList)
    {
      clientServiceGraphic.GetSubTags(out requestID, propertyPath, out propertyList);
    }

    public bool ModifyItem(out Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return clientServiceGraphic.ModifyItem(out requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyItemPath(out Int64 requestID, Guid guid, String path)
    {
      return clientServiceGraphic.ModifyItemPath(out requestID, guid, path);
    }

    public bool ModifyLink(out Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return clientServiceGraphic.ModifyLink(out requestID, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool ModifyThing(out Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
    {
      return clientServiceGraphic.ModifyThing(out requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
    }

    public bool ModifyThingPath(out Int64 requestID, Guid guid, String path)
    {
      return clientServiceGraphic.ModifyThingPath(out requestID, guid, path);
    }

    public PortStatus PortCheck(out Int64 requestID, Guid itemGuid, Anchor anchor)
    {
      return clientServiceGraphic.PortCheck(out requestID, itemGuid, anchor);
    }

    public ArrayList PropertyList(out Int64 requestID, Guid guid, String tag, String path)
    {
      return clientServiceGraphic.PropertyList(out requestID, guid, tag, path);
    }

    public void ServiceGraphicItemCreated(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {

      if (!graphicItems.ContainsKey(guid))
      {
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        OnItemCreated(eventId, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicItemDeleted(Int64 eventId, Int64 requestID, Guid guid)
    {

      if (graphicItems.ContainsKey(guid))
      {
        graphicItems.Remove(guid);

        OnItemDeleted(eventId, requestID, guid);
      }
    }

    public void ServiceGraphicItemModified(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
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
        graphicItem.FillColor = fillColor;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        OnItemModified(eventId, requestID, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicLinkCreated(Int64 eventId, Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {

      if (!graphicLinks.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag, classId, origin, originPort, destination, destinationPort, controlPoints);
        graphicLinks.Add(guid, graphicLink);

        OnLinkCreated(eventId, requestID, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void ServiceGraphicLinkDeleted(Int64 eventId, Int64 requestID, Guid guid)
    {

      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        OnLinkDeleted(eventId, requestID, guid);
      }
    }

    public void ServiceGraphicLinkModified(Int64 eventId, Int64 requestID, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
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

        foreach (PointF controlPoint in controlPoints)
          graphicLink.ControlPoints.Add(controlPoint);

        OnLinkModified(eventId, requestID, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void ServiceGraphicStateChanged(Int64 eventId, Int64 requestID, RunStates runState)
    {
      OnStateChanged(eventId, requestID, runState);
    }

    public void ServiceGraphicStep(Int64 eventId, Int64 step, DateTime time)
    {
      OnStep(eventId, step, time);
    }

    public void ServiceGraphicSync(Int64 eventId)
    {
      OnSync(eventId);
    }

    public void ServiceGraphicThingCreated(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
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

        OnThingCreated(eventId, requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void ServiceGraphicThingDeleted(Int64 eventId, Int64 requestID, Guid guid)
    {

      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        OnThingDeleted(eventId, requestID, guid);
      }
    }

    public void ServiceGraphicThingModified(Int64 eventId, Int64 requestID, Guid guid, String tag, String path, RectangleF boundingRect, String xaml, Single angle, bool mirrorX, bool mirrorY)
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

        OnThingModified(eventId, requestID, guid, tag, path, boundingRect, xaml, angle, mirrorX, mirrorY);
      }
    }

    public void Syncxxx()
    {
      MemoryStream memoryStream;
      BinaryFormatter bf = new BinaryFormatter();

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, clientServiceGraphic.graphicLinks);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicLinks = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicLink>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, clientServiceGraphic.graphicItems);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicItems = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicItem>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, clientServiceGraphic.graphicThings);
      memoryStream.Seek(0, SeekOrigin.Begin);
      graphicThings = bf.Deserialize(memoryStream) as Dictionary<Guid, GraphicThing>;
    }

    public bool TestUrl(Uri url)
    {

      try
      {
        clientServiceGraphic = Activator.GetObject(typeof(BaseProtocol), url.ToString()) as ClientServiceProtocol;

        Name = clientServiceGraphic.Name; // Force a test of the connection.

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

      if (clientServiceGraphic != null)
      {

        try
        {

          if (serviceGraphicStateChangedHandler != null) clientServiceGraphic.StateChanged -= serviceGraphicStateChangedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicStepHandler != null) clientServiceGraphic.Step -= serviceGraphicStepHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicSyncHandler != null) clientServiceGraphic.Sync -= serviceGraphicSyncHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicItemCreatedHandler != null) clientServiceGraphic.ItemCreated -= serviceGraphicItemCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicItemModifiedHandler != null) clientServiceGraphic.ItemModified -= serviceGraphicItemModifiedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicItemDeletedHandler != null) clientServiceGraphic.ItemDeleted -= serviceGraphicItemDeletedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicLinkCreatedHandler != null) clientServiceGraphic.LinkCreated -= serviceGraphicLinkCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicLinkModifiedHandler != null) clientServiceGraphic.LinkModified -= serviceGraphicLinkModifiedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicLinkDeletedHandler != null) clientServiceGraphic.LinkDeleted -= serviceGraphicLinkDeletedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingCreatedHandler != null) clientServiceGraphic.ThingCreated -= serviceGraphicThingCreatedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingModifiedHandler != null) clientServiceGraphic.ThingModified -= serviceGraphicThingModifiedHandler;
        }

        catch (InvalidOperationException) { }

        try
        {

          if (serviceGraphicThingDeletedHandler != null) clientServiceGraphic.ThingDeleted -= serviceGraphicThingDeletedHandler;
        }

        catch (InvalidOperationException) { }
      }
    }
  }
}
