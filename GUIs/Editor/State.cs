using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Interface;
using SysCAD;
using System.Collections;
using System.Drawing.Drawing2D;

namespace SysCAD.Editor
{
  public class State
  {
    private Dictionary<Guid, Link> links = new Dictionary<Guid, Link>();
    private Dictionary<Guid, Item> items = new Dictionary<Guid, Item>();
    private Dictionary<Guid, Thing> things = new Dictionary<Guid, Thing>();

    private ClientGraphic graphic;
    private Config config;

    private PureComponents.TreeView.TreeView tvNavigation;

    public bool ShowModels = false;
    public bool ShowGraphics = true;
    public bool ShowLinks = true;
    public bool ShowTags = true;

    public bool SelectLinks = true;
    public bool SelectItems = true;

    public IEnumerable<GraphicStencil> GraphicStencils
    {
      get { return config.graphicStencils.Values; }
    }

    public IEnumerable<ModelStencil> ModelStencils
    {
      get { return config.modelStencils.Values; }
    }

    public Config Config
    {
      set { config = value; }
    }

    public ClientGraphic Graphic
    {
      set { graphic = value; }
    }

    public PureComponents.TreeView.TreeView TvNavigation
    {
      set { tvNavigation = value; }
    }

    public Arrow Arrow(Guid guid)
    {
      Link link;
      links.TryGetValue(guid, out link);
      return link.Arrow;
    }

    private Item item(Guid guid)
    {
      Item item;
      items.TryGetValue(guid, out item);
      return item;
    }

    public void setArrow(Guid guid, String tag, Arrow arrow, GraphicLink graphicLink)
    {
      Link link = new Link(guid, tag, graphicLink);
      link.Arrow = arrow;
      links.Add(guid, link);
    }

    internal void Remove(FlowChart flowChart)
    {
      BoxCollection boxSelection = flowChart.Selection.Boxes.Clone();

      foreach (Box box in boxSelection)
      {
        Item item = box.Tag as Item;
        if (item != null)
        {
          item.Remove(flowChart);
          items.Remove(item.Guid);
        }
      }

      ArrowCollection arrowSelection = flowChart.Selection.Arrows.Clone();

      foreach (Arrow arrow in arrowSelection)
      {
        Link link = arrow.Tag as Link;
        if (link != null)
        {
          link.Remove(flowChart);
          links.Remove(link.Guid);
        }
      }
    }

    internal void CreateItem(GraphicItem graphicItem, bool isVisible, FlowChart flowchart)
    {
      flowchart.SuspendLayout();

      ModelStencil modelStencil;
      GraphicStencil graphicStencil;

      Box modelBox = flowchart.CreateBox(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
      modelBox.RotationAngle = graphicItem.Angle;
      modelBox.ToolTip = graphicItem.Tag + "\n\nClassID: " + graphicItem.Model;
      modelBox.Style = BoxStyle.Shape;

      if (config.modelStencils.TryGetValue(graphicItem.Model, out modelStencil))
        modelBox.Shape = GetShapeTemplate(modelStencil, graphicItem.MirrorX, graphicItem.MirrorY);
      else
        modelBox.Shape = ShapeTemplate.FromId("Decision2");

      modelBox.AnchorPattern = GetAnchorPattern(modelStencil, graphicItem);

      modelBox.FillColor = Color.FromArgb(150, System.Drawing.Color.BurlyWood);
      modelBox.FrameColor = Color.FromArgb(200, System.Drawing.Color.BurlyWood);
      modelBox.Visible = ShowModels && isVisible;

      Box graphicBox = flowchart.CreateBox(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
      graphicBox.AttachTo(modelBox, 0, 0, 100, 100);
      graphicBox.RotationAngle = graphicItem.Angle;
      graphicBox.ToolTip = graphicItem.Tag + "\n\nClassID: " + graphicItem.Model;
      graphicBox.Style = BoxStyle.Shape;

      if (config.graphicStencils.TryGetValue(graphicItem.Shape, out graphicStencil))
        graphicBox.Shape = GetShapeTemplate(graphicStencil, graphicItem.MirrorX, graphicItem.MirrorY);
      else
        graphicBox.Shape = ShapeTemplate.FromId("Decision2");

      graphicBox.EnabledHandles = Handles.None;
      graphicBox.HandlesStyle = HandlesStyle.Invisible;
      graphicBox.Visible = ShowGraphics && isVisible;


      if (graphicItem.FillColor.IsEmpty)
        graphicItem.FillColor = graphicBox.FillColor;
      else
        graphicBox.FillColor = graphicItem.FillColor;


      RectangleF textArea = graphicStencil.textArea;
      RectangleF textBoxRect = new RectangleF(
                                graphicItem.X + textArea.X / graphicStencil.defaultSize.Width * graphicItem.Width,
                                graphicItem.Y + textArea.Y / graphicStencil.defaultSize.Height * graphicItem.Height,
                                textArea.Width / graphicStencil.defaultSize.Width * graphicItem.Width,
                                textArea.Height / graphicStencil.defaultSize.Height * graphicItem.Height);

      Box textBox = flowchart.CreateBox(textBoxRect.X, textBoxRect.Y, textBoxRect.Width, textBoxRect.Height);
      textBox.AttachTo(modelBox, AttachToNode.BottomCenter);
      textBox.FillColor = Color.FromArgb(0, System.Drawing.Color.Black);
      textBox.FrameColor = Color.FromArgb(0, System.Drawing.Color.Black);
      textBox.Style = BoxStyle.Shape;
      textBox.Shape = ShapeTemplate.FromId("Rectangle");
      textBox.EnabledHandles = Handles.ResizeTopLeft | Handles.ResizeTopRight |
        Handles.ResizeBottomRight | Handles.ResizeBottomLeft | Handles.ResizeTopCenter |
        Handles.ResizeMiddleRight | Handles.ResizeBottomCenter | Handles.ResizeMiddleLeft |
        Handles.Move;
      textBox.Visible = ShowTags && isVisible;
      textBox.Text = graphicItem.Tag;


      Item item = new Item(graphicItem.Guid, graphicItem.Tag, modelBox, graphicBox, textBox, isVisible, graphicItem);


      modelBox.Tag = item;
      graphicBox.Tag = item;
      textBox.Tag = item;

      items.Add(item.Guid, item);

      flowchart.ResumeLayout();
    }

    internal void CreateThing(GraphicThing graphicThing, bool isVisible, FlowChart flowchart)
    {
      flowchart.SuspendLayout();

      Box box = flowchart.CreateBox(graphicThing.X, graphicThing.Y, graphicThing.Width, graphicThing.Height);
      box.RotationAngle = graphicThing.Angle;
      box.ToolTip = graphicThing.Tag;
      box.Style = BoxStyle.Shape;

      box.Shape = GetShapeTemplate(graphicThing);

      box.Text = graphicThing.Text;

      if (!graphicThing.FillColor.IsEmpty)
        box.FillColor = Color.FromArgb(150, graphicThing.FillColor);
      if (!graphicThing.FillColor.IsEmpty)
        box.FrameColor = Color.FromArgb(200, graphicThing.FrameColor);
      box.Visible = isVisible;

      box.ZBottom();

      Thing thing = new Thing(graphicThing.Guid, graphicThing.Tag, box, isVisible, graphicThing);

      box.Tag = thing;

      things.Add(thing.Guid, thing);

      flowchart.ResumeLayout();
    }

    public AnchorPattern GetAnchorPattern(ModelStencil modelStencil, GraphicItem graphicItem)
    {
      AnchorPointCollection anchorPointCollection = new AnchorPointCollection();
      if (modelStencil.Anchors != null)
      {
        graphicItem.anchorIntToTag.Clear();
        graphicItem.anchorTagToInt.Clear();
        int anchorInt = 0;
        foreach (Anchor anchor in modelStencil.Anchors)
        {
          graphicItem.anchorIntToTag.Add(anchorInt, anchor.tag);
          graphicItem.anchorTagToInt.Add(anchor.tag, anchorInt);
          anchorInt++;
          float x = anchor.position.X;
          if (graphicItem.MirrorX) x = 100.0F - x;
          float y = anchor.position.Y;
          if (graphicItem.MirrorY) y = 100.0F - y;
          AnchorPoint anchorPoint = new AnchorPoint((short)x, (short)y, true, true, MarkStyle.Circle, Color.Green);
          anchorPoint.Tag = anchor;
          anchorPointCollection.Add(anchorPoint);
        }
      }
      return new AnchorPattern(anchorPointCollection);
    }

    internal bool DeleteItem(Guid guid)
    {
      //TBD: unlink connected links first
      return items.Remove(guid);
    }

    internal bool DeleteThing(Guid guid)
    {
      return things.Remove(guid);
    }

    public void SetControlPoints(Arrow arrow, List<PointF> points)
    {
      arrow.SegmentCount = (short)(points.Count - 1);
      int i = 0;
      PointF keepPoint = PointF.Empty;
      foreach (PointF point in points)
      {
        arrow.ControlPoints[i++] = point;
        keepPoint = point;
      }

      while (i <= arrow.SegmentCount)
      {
        arrow.ControlPoints[i++] = keepPoint;
      }
      arrow.UpdateFromPoints();
    }

    internal void CreateLink(GraphicLink graphicLink, bool isVisible, FlowChart flowchart)
    {
      Arrow arrow = flowchart.CreateArrow(new PointF(0.0F, 0.0F), new PointF(10.0F, 10.0F));

      switch (graphicLink.ClassID)
      {
        case "Pipe-1":
          break;
        case "CtrlLink":
          arrow.PenColor = Color.Gray;
          break;
        default:
          arrow.PenColor = Color.Red;
          break;
      }

      Item origin = null;
      Item destination = null;

      if (graphicLink.Origin != null) origin = item(graphicLink.Origin);
      if (graphicLink.Destination != null) destination = item(graphicLink.Destination);

      PointF pointOrigin = new PointF();
      PointF pointDestination = new PointF();

      if (graphicLink.controlPoints != null && graphicLink.controlPoints.Count > 1)
      {
        pointOrigin = graphicLink.controlPoints[0];
        pointDestination = graphicLink.controlPoints[graphicLink.controlPoints.Count - 1];
      }

      if (origin != null)
        arrow.Origin = origin.Model;
      if (destination != null)
        arrow.Destination = destination.Model;



      if ((graphicLink.OriginPort != null) && ((origin.Model.Tag as Item).GraphicItem.anchorTagToInt.ContainsKey(graphicLink.OriginPort)))
        arrow.OrgnAnchor = (origin.Model.Tag as Item).GraphicItem.anchorTagToInt[graphicLink.OriginPort];
      else
        arrow.OrgnAnchor = -1;

      if ((graphicLink.DestinationPort != null) && ((destination.Model.Tag as Item).GraphicItem.anchorTagToInt.ContainsKey(graphicLink.DestinationPort)))
        arrow.DestAnchor = (destination.Model.Tag as Item).GraphicItem.anchorTagToInt[graphicLink.DestinationPort];
      else
        arrow.DestAnchor = -1;


      arrow.ToolTip = "Tag:" + graphicLink.Tag +
        "\nSrc: " + origin.Tag + ":" + graphicLink.OriginPort +
        "\nDst: " + destination.Tag + ":" + graphicLink.DestinationPort;
      arrow.ArrowHead = ArrowHead.Triangle;
      arrow.Style = ArrowStyle.Cascading;

      if (graphicLink.controlPoints != null && graphicLink.controlPoints.Count > 1)
      {
        SetControlPoints(arrow, graphicLink.controlPoints);
      }

      Link link = new Link(graphicLink.Guid, graphicLink.Tag, graphicLink);
      link.Arrow = arrow;
      link.Visible = true;

      arrow.Tag = link;

      arrow.Visible = ShowLinks && isVisible;

      links.Add(link.Guid, link);
    }

    internal bool DeleteLink(Guid guid)
    {
      return links.Remove(guid);
    }



    internal void ItemVisible(Guid guid, bool visible)
    {
      Item item;
      if (items.TryGetValue(guid, out item))
      {
        item.Visible = visible;
        item.Model.Visible = visible && (item.Model.Selected || ShowModels);
        item.Graphic.Visible = visible && ShowGraphics;
        item.Text.Visible = visible && ShowTags;

        foreach (Arrow arrowDestination in item.Model.IncomingArrows)
        {
          arrowDestination.Visible = visible && ShowLinks;
        }

        foreach (Arrow arrowOrigin in item.Model.OutgoingArrows)
        {
          arrowOrigin.Visible = visible && ShowLinks;
        }
      }

      Thing thing;
      if (things.TryGetValue(guid, out thing))
      {
        thing.Visible = visible;
        thing.Box.Visible = visible;
      }
    }

    internal void ItemSelected(Guid guid, bool selected)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        item.Selected = selected;
        item.Model.Selected = selected && item.Visible;
      }
    }

    internal bool Exists(Guid guid)
    {
      Item item;
      items.TryGetValue(guid, out item);

      Link link;
      links.TryGetValue(guid, out link);

      Thing thing;
      things.TryGetValue(guid, out thing);

      return ((link != null) || (item != null) || (thing != null));
    }

    internal bool Exists(String tag)
    {
      foreach (Item item in items.Values)
      {
        if (item.Tag == tag)
          return true;
      }

      foreach (Link link in links.Values)
      {
        if (link.Tag == tag)
          return true;
      }

      foreach (Thing thing in things.Values)
      {
        if (thing.Tag == tag)
          return true;
      }

      return false;
    }

    internal void SetAngle(Guid guid, float angle)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        item.Model.RotationAngle = angle;
        item.Graphic.RotationAngle = angle;
      }
    }

    internal void SetHeight(Guid guid, float height)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Height = height;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetWidth(Guid guid, float width)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Width = width;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetX(Guid guid, float x)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.X = x;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetY(Guid guid, float y)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Y = y;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetMirrorX(Guid guid, bool mirrorX)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        GraphicItem graphicItem;
        if (graphic.graphicItems.TryGetValue(guid, out graphicItem))
        {
          GraphicStencil stencil;
          if (config.graphicStencils.TryGetValue(graphicItem.Shape, out stencil))
          {
            item.Graphic.Shape = GetShapeTemplate(stencil, graphicItem.MirrorX, graphicItem.MirrorY);
          }
        }
      }
    }

    internal void SetMirrorY(Guid guid, bool mirrorY)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        GraphicItem graphicItem;
        if (graphic.graphicItems.TryGetValue(guid, out graphicItem))
        {
          GraphicStencil stencil;
          if (config.graphicStencils.TryGetValue(graphicItem.Shape, out stencil))
          {
            item.Graphic.Shape = GetShapeTemplate(stencil, graphicItem.MirrorX, graphicItem.MirrorY);
          }
        }
      }
    }

    internal void SetTag(Guid guid, String tag)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        item.Tag = tag;
        item.Text.Text = tag;
      }
    }

    internal void SetFillColor(Guid guid, Color fillColor)
    {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
      {
        item.Graphic.FillColor = fillColor;
      }
    }

    internal ModelStencil ModelShape(string stencilName)
    {
      ModelStencil modelStencil;
      config.modelStencils.TryGetValue(stencilName, out modelStencil);
      return modelStencil;
    }

    internal GraphicStencil GraphicShape(string stencilName)
    {
      GraphicStencil graphicStencil;
      config.graphicStencils.TryGetValue(stencilName, out graphicStencil);
      return graphicStencil;
    }

    internal GraphicItem GraphicItem(Guid guid)
    {
      GraphicItem graphicItem;
      graphic.graphicItems.TryGetValue(guid, out graphicItem);
      return graphicItem;
    }

    internal GraphicItem GraphicItem(Box box)
    {
      GraphicItem graphicItem = null;
      if (box.Tag is Item)
        graphic.graphicItems.TryGetValue((box.Tag as Item).Guid, out graphicItem);
      return graphicItem;
    }

    internal GraphicLink GraphicLink(Guid guid)
    {
      GraphicLink graphicLink;
      graphic.graphicLinks.TryGetValue(guid, out graphicLink);
      return graphicLink;
    }

    internal GraphicLink GraphicLink(Arrow arrow)
    {
      GraphicLink graphicLink = null;
      if ((arrow != null) && (arrow.Tag != null))
        graphic.graphicLinks.TryGetValue((arrow.Tag as Link).Guid, out graphicLink);
      return graphicLink;
    }

    internal GraphicThing GraphicThing(Guid guid)
    {
      GraphicThing graphicThing;
      graphic.graphicThings.TryGetValue(guid, out graphicThing);
      return graphicThing;
    }

    internal GraphicThing GraphicThing(Box box)
    {
      GraphicThing graphicThing = null;
      if (box.Tag is Thing)
        graphic.graphicThings.TryGetValue((box.Tag as Thing).Guid, out graphicThing);
      return graphicThing;
    }

    internal IEnumerable<GraphicItem> GraphicItems
    {
      get { return graphic.graphicItems.Values; }
    }

    internal IEnumerable<GraphicLink> GraphicLinks
    {
      get { return graphic.graphicLinks.Values; }
    }

    internal IEnumerable<GraphicThing> GraphicThings
    {
      get { return graphic.graphicThings.Values; }
    }

    internal Item Item(Guid guid)
    {
      Item item;
      items.TryGetValue(guid, out item);
      return item;
    }

    internal Link Link(Guid guid)
    {
      Link link;
      links.TryGetValue(guid, out link);
      return link;
    }

    internal Thing Thing(Guid guid)
    {
      Thing thing;
      things.TryGetValue(guid, out thing);
      return thing;
    }

    internal IEnumerable<Item> Items
    {
      get { return items.Values; }
    }

    internal IEnumerable<Link> Links
    {
      get { return links.Values; }
    }

    internal string CurrentPath
    {
      get { return tvNavigation.SelectedNode.FullPath; }
    }


    internal bool IsItem(Guid guid)
    {
      return graphic.graphicItems.ContainsKey(guid);
    }



    internal bool ModifyGraphicItem(out uint requestID, Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      return graphic.ModifyItem(out requestID, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    internal bool CreateGraphicItem(out uint requestID, out Guid guid, String tag, String path, String model, String shape, RectangleF boundingRect, Single angle, Color fillColor, bool mirrorX, bool mirrorY)
    {
      return graphic.CreateItem(out requestID, out guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    internal bool DeleteGraphicItem(out uint requestID, Guid guid)
    {
      return graphic.DeleteItem(out requestID, guid);
    }


    internal bool ModifyGraphicLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return graphic.ModifyLink(out requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    internal bool CreateGraphicLink(out uint requestID, Guid guid, String tag, String classID, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      return graphic.CreateLink(out requestID, guid, tag, classID, origin, destination, originPort, destinationPort, controlPoints);
    }

    internal bool DeleteGraphicLink(out uint requestID, Guid guid)
    {
      return graphic.DeleteLink(out requestID, guid);
    }


    internal bool ModifyGraphicThing(out uint requestID, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      return graphic.ModifyThing(out requestID, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    internal bool CreateGraphicThing(out uint requestID, out Guid guid, String tag, String path, RectangleF boundingRect, Single angle, Color fillColor, bool mirrorX, bool mirrorY)
    {
      return graphic.CreateThing(out requestID, out guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    internal bool DeleteGraphicThing(out uint requestID, Guid guid)
    {
      return graphic.DeleteThing(out requestID, guid);
    }


    internal PortStatus PortCheck(Guid itemGuid, Anchor anchor)
    {
      if (anchor != null)
        return graphic.PortCheck(itemGuid, anchor);
      else
        return PortStatus.Unavailable;
    }


    internal ArrayList PropertyList(Guid guid, String tag, String path)
    {
      return graphic.PropertyList(guid, tag, path);
    }


    internal void ConnectGraphic(
      ClientGraphic.ItemCreatedHandler itemCreatedHandler,
      ClientGraphic.ItemModifiedHandler itemModifiedHandler,
      ClientGraphic.ItemDeletedHandler itemDeletedHandler,
      ClientGraphic.LinkCreatedHandler linkCreatedHandler,
      ClientGraphic.LinkModifiedHandler linkModifiedHandler,
      ClientGraphic.LinkDeletedHandler linkDeletedHandler,
      ClientGraphic.ThingCreatedHandler thingCreatedHandler,
      ClientGraphic.ThingModifiedHandler thingModifiedHandler,
      ClientGraphic.ThingDeletedHandler thingDeletedHandler)
    {
      graphic.ItemCreated += itemCreatedHandler;
      graphic.ItemModified += itemModifiedHandler;
      graphic.ItemDeleted += itemDeletedHandler;

      graphic.LinkCreated += linkCreatedHandler;
      graphic.LinkModified += linkModifiedHandler;
      graphic.LinkDeleted += linkDeletedHandler;

      graphic.ThingCreated += thingCreatedHandler;
      graphic.ThingModified += thingModifiedHandler;
      graphic.ThingDeleted += thingDeletedHandler;
    }


    internal void DisconnectGraphic(
      ClientGraphic.ItemCreatedHandler itemCreatedHandler,
      ClientGraphic.ItemModifiedHandler itemModifiedHandler,
      ClientGraphic.ItemDeletedHandler itemDeletedHandler,
      ClientGraphic.LinkCreatedHandler linkCreatedHandler,
      ClientGraphic.LinkModifiedHandler linkModifiedHandler,
      ClientGraphic.LinkDeletedHandler linkDeletedHandler,
      ClientGraphic.ThingCreatedHandler thingCreatedHandler,
      ClientGraphic.ThingModifiedHandler thingModifiedHandler,
      ClientGraphic.ThingDeletedHandler thingDeletedHandler)
    {
      graphic.ItemCreated -= itemCreatedHandler;
      graphic.ItemModified -= itemModifiedHandler;
      graphic.ItemDeleted -= itemDeletedHandler;

      graphic.LinkCreated -= linkCreatedHandler;
      graphic.LinkModified -= linkModifiedHandler;
      graphic.LinkDeleted -= linkDeletedHandler;

      graphic.ThingCreated -= thingCreatedHandler;
      graphic.ThingModified -= thingModifiedHandler;
      graphic.ThingDeleted -= thingDeletedHandler;
    }



    internal void AddNode(string path, string tag, Guid guid)
    {
      tvNavigation.GetNodeByPath(path).Nodes.Add(tag, guid.ToString());
    }

    internal float Mirrored(float x, bool mirrored)
    {
      if (mirrored)
        return 100.0F - x;
      else
        return x;
    }

    internal ArcTemplate MirroredArc(Arc arc, bool mirrorX, bool mirrorY)
    {
      return new ArcTemplate(Mirrored(arc.x, mirrorX), Mirrored(arc.y, mirrorY),
                             arc.w, arc.h, arc.a, arc.s);
    }

    internal LineTemplate MirroredLine(Line line, bool mirrorX, bool mirrorY)
    {
      return new LineTemplate(Mirrored(line.x1, mirrorX), Mirrored(line.y1, mirrorY),
                              Mirrored(line.x2, mirrorX), Mirrored(line.y2, mirrorY));
    }

    internal BezierTemplate MirroredBezier(Bezier bezier, bool mirrorX, bool mirrorY)
    {
      return new BezierTemplate(Mirrored(bezier.x1, mirrorX), Mirrored(bezier.y1, mirrorY),
                                Mirrored(bezier.x2, mirrorX), Mirrored(bezier.y2, mirrorY),
                                Mirrored(bezier.x3, mirrorX), Mirrored(bezier.y3, mirrorY),
                                Mirrored(bezier.x4, mirrorX), Mirrored(bezier.y4, mirrorY));
    }

    public ShapeTemplate GetShapeTemplate(ModelStencil stencil, bool mirrorX, bool mirrorY)
    {
      int i;

      ElementTemplate[] elementTemplate = new ElementTemplate[stencil.elements.Count];
      i = 0;
      foreach (Element element in stencil.elements)
      {
        if (element is Arc) elementTemplate[i] = MirroredArc(element as Arc, mirrorX, mirrorY);
        if (element is Line) elementTemplate[i] = MirroredLine(element as Line, mirrorX, mirrorY);
        if (element is Bezier) elementTemplate[i] = MirroredBezier(element as Bezier, mirrorX, mirrorY);
        i++;
      }

      ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.decorations.Count];
      i = 0;
      foreach (Element decoration in stencil.decorations)
      {
        if (decoration is Arc) decorationTemplate[i] = MirroredArc(decoration as Arc, mirrorX, mirrorY);
        if (decoration is Line) decorationTemplate[i] = MirroredLine(decoration as Line, mirrorX, mirrorY);
        if (decoration is Bezier) decorationTemplate[i] = MirroredBezier(decoration as Bezier, mirrorX, mirrorY);
        i++;
      }

      return (new ShapeTemplate(elementTemplate, decorationTemplate, null, stencil.fillMode, stencil.Tag));
    }


    public ShapeTemplate GetShapeTemplate(OldGraphicStencil stencil, bool mirrorX, bool mirrorY)
    {
      int i;

      ElementTemplate[] elementTemplate = new ElementTemplate[stencil.elements.Count];
      i = 0;
      foreach (Element element in stencil.elements)
      {
        if (element is Arc) elementTemplate[i] = MirroredArc(element as Arc, mirrorX, mirrorY);
        if (element is Line) elementTemplate[i] = MirroredLine(element as Line, mirrorX, mirrorY);
        if (element is Bezier) elementTemplate[i] = MirroredBezier(element as Bezier, mirrorX, mirrorY);
        i++;
      }

      ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.decorations.Count];
      i = 0;
      foreach (Element decoration in stencil.decorations)
      {
        if (decoration is Arc) decorationTemplate[i] = MirroredArc(decoration as Arc, mirrorX, mirrorY);
        if (decoration is Line) decorationTemplate[i] = MirroredLine(decoration as Line, mirrorX, mirrorY);
        if (decoration is Bezier) decorationTemplate[i] = MirroredBezier(decoration as Bezier, mirrorX, mirrorY);
        i++;
      }

      return (new ShapeTemplate(elementTemplate, decorationTemplate, new ElementTemplate[0], stencil.fillMode, stencil.Tag));
    }

    public ShapeTemplate GetShapeTemplate(GraphicStencil stencil, bool mirrorX, bool mirrorY)
    {
      int i;

      ElementTemplate[] elementTemplate = new ElementTemplate[stencil.elements.Count];
      i = 0;
      foreach (Element element in stencil.elements)
      {
        if (element is Arc) elementTemplate[i] = MirroredArc(element as Arc, mirrorX, mirrorY);
        if (element is Line) elementTemplate[i] = MirroredLine(element as Line, mirrorX, mirrorY);
        if (element is Bezier) elementTemplate[i] = MirroredBezier(element as Bezier, mirrorX, mirrorY);
        i++;
      }

      ElementTemplate[] decorationTemplate = new ElementTemplate[stencil.decorations.Count];
      i = 0;
      foreach (Element decoration in stencil.decorations)
      {
        if (decoration is Arc) decorationTemplate[i] = MirroredArc(decoration as Arc, mirrorX, mirrorY);
        if (decoration is Line) decorationTemplate[i] = MirroredLine(decoration as Line, mirrorX, mirrorY);
        if (decoration is Bezier) decorationTemplate[i] = MirroredBezier(decoration as Bezier, mirrorX, mirrorY);
        i++;
      }

      return (new ShapeTemplate(elementTemplate, decorationTemplate, new ElementTemplate[0], stencil.fillMode, stencil.Tag));
    }

    public ShapeTemplate GetShapeTemplate(GraphicThing thing)
    {
      int i;

      ElementTemplate[] elementTemplate = null;
      ElementTemplate[] decorationTemplate = null;
      ElementTemplate[] textAreaTemplate = null;
      
      if (thing != null)
      {
        if (thing.elements != null)
        {
          elementTemplate = new ElementTemplate[thing.elements.Count];
          i = 0;
          foreach (Element element in thing.elements)
          {
            if (element is Arc) elementTemplate[i] = MirroredArc(element as Arc, thing.MirrorX, thing.MirrorY);
            if (element is Line) elementTemplate[i] = MirroredLine(element as Line, thing.MirrorX, thing.MirrorY);
            if (element is Bezier) elementTemplate[i] = MirroredBezier(element as Bezier, thing.MirrorX, thing.MirrorY);
            i++;
          }
        }

        if (thing.decorations != null)
        {
          decorationTemplate = new ElementTemplate[thing.decorations.Count];
          i = 0;
          foreach (Element decoration in thing.decorations)
          {
            if (decoration is Arc) decorationTemplate[i] = MirroredArc(decoration as Arc, thing.MirrorX, thing.MirrorY);
            if (decoration is Line) decorationTemplate[i] = MirroredLine(decoration as Line, thing.MirrorX, thing.MirrorY);
            if (decoration is Bezier) decorationTemplate[i] = MirroredBezier(decoration as Bezier, thing.MirrorX, thing.MirrorY);
            i++;
          }
        }

        if (thing.textArea != null)
        {
          textAreaTemplate = new ElementTemplate[thing.textArea.Count];
          i = 0;
          foreach (Element textArea in thing.textArea)
          {
            if (textArea is Arc) textAreaTemplate[i] = MirroredArc(textArea as Arc, thing.MirrorX, thing.MirrorY);
            if (textArea is Line) textAreaTemplate[i] = MirroredLine(textArea as Line, thing.MirrorX, thing.MirrorY);
            if (textArea is Bezier) textAreaTemplate[i] = MirroredBezier(textArea as Bezier, thing.MirrorX, thing.MirrorY);
            i++;
          }
        }
      }

      return (new ShapeTemplate(elementTemplate, decorationTemplate, textAreaTemplate, thing.fillMode, thing.Tag));
    }

  }
}
