using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Interface;

namespace SysCAD.Editor
  {
  public class Link
    {
    private Guid guid;

    private String tag;

    private Arrow arrow;

    private GraphicLink graphicLink;

    private bool visible;

    public Guid Guid
      {
      get { return guid; }
      }

    public String Tag
      {
      get { return tag; }
      }

    public Arrow Arrow
      {
      get { return arrow; }
      set { arrow = value; }
      }

    public bool Visible
      {
      get { return visible; }
      set { visible = value; }
      }

    public Link(Guid guid, String tag)
      {
      this.guid = guid;
      this.tag = tag;
      }
    }

  public class Item
    {
    private Guid guid;
    private String tag;
    private Box model;
    private Box graphic;
    private PureComponents.TreeView.Node node;

    private GraphicItem graphicItem;

    private bool visible;
    private bool selected;

    public Guid Guid
      {
      get { return guid; }
      }

    public String Tag
      {
      get { return tag; }
      set { tag = value; }
      }

    public Box Model
      {
      get { return model; }
      set { model = value; }
      }

    public Box Graphic
      {
      get { return graphic; }
      set { graphic = value; }
      }

    public PureComponents.TreeView.Node Node
      {
      get { return node; }
      set { node = value; }
      }

    public bool Visible
      {
      get { return visible; }
      set { visible = value; }
      }

    public bool Selected
      {
      get { return selected; }
      set { selected = value; }
      }

    public ArrowCollection IncomingArrows
      {
      get { return model.IncomingArrows; }
      }

    public ArrowCollection OutgoingArrows
      {
      get { return model.OutgoingArrows; }
      }

    public Item(String tag, Box model, Box graphic, bool visible)
      {
      this.guid = Guid.NewGuid();
      this.tag = tag;
      this.model = model;
      this.graphic = graphic;
      this.visible = visible;
      }

    public Item(Guid guid, String tag, Box model, Box graphic, bool visible)
      {
      this.guid = guid;
      this.tag = tag;
      this.model = model;
      this.graphic = graphic;
      this.visible = visible;
      }
    }

  //---------------------------------------------------------

  public class State
    {
    private Dictionary<Guid, Link> links = new Dictionary<Guid, Link>();
    private Dictionary<Guid, Item> items = new Dictionary<Guid, Item>();

    private Graphic graphic;
    private Config config;

    private PureComponents.TreeView.TreeView tvNavigation;

    public bool ShowModels = false;
    public bool ShowGraphics = true;
    public bool ShowLinks = true;
    public bool ShowTags = true;

    public bool SelectLinks = true;
    public bool SelectItems = true;

    private Box justCreatedModelBox;

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

    public Graphic Graphic
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

    public Item item(Guid guid)
      {
      Item item;
      items.TryGetValue(guid, out item);
      return item;
      }

    public void setArrow(Guid guid, String tag, Arrow arrow)
      {
      Link link = new Link(guid, tag);
      link.Arrow = arrow;
      links.Add(guid, link);
      }

    internal void newGraphicLink(GraphicLink graphicLink, Arrow arrow, bool visible)
      {
      Item origin = null;
      Item destination = null;

      if (graphicLink.Source != null) origin = item(graphicLink.Source);
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

      arrow.Text = graphicLink.Tag;
      arrow.ToolTip = graphicLink.Tag + "\n\nSrc: " + graphicLink.Source + "\nDst: " + graphicLink.Destination;
      arrow.ArrowHead = ArrowHead.Triangle;
      arrow.Style = ArrowStyle.Cascading;

      if (graphicLink.controlPoints != null && graphicLink.controlPoints.Count > 1)
        {
        arrow.SegmentCount = (short)(graphicLink.controlPoints.Count - 1);
        int i = 0;
        foreach (PointF point in graphicLink.controlPoints)
          arrow.ControlPoints[i++] = point;
        arrow.UpdateFromPoints();
        PointCollection a = arrow.ControlPoints;
        }

      Link link = new Link(graphicLink.Guid, graphicLink.Tag);
      link.Arrow = arrow;
      link.Visible = true;

      arrow.Tag = link;

      arrow.Visible = ShowLinks && visible;

      links.Add(link.Guid, link);
      }

    internal void Remove(Item item, FlowChart flowchart)
      {
      if (item != null)
        {
        if (item.Model != null)
          flowchart.DeleteObject(item.Model);
        if (item.Graphic != null)
          flowchart.DeleteObject(item.Graphic);
        if (item.Node != null)
          item.Node.Remove();
        }
      }

    internal void newItem(GraphicItem graphicItem, bool isVisible, FlowChart flowchart)
      {
      Box modelBox = flowchart.CreateBox(0.0F, 0.0F, 10.0F, 10.0F);
      Box graphicBox = flowchart.CreateBox(0.0F, 0.0F, 10.0F, 10.0F);

      modelBox.BoundingRect = new RectangleF(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
      modelBox.RotationAngle = graphicItem.Angle;
      modelBox.ToolTip = graphicItem.Tag + "\n\nClassID: " + graphicItem.Model;
      modelBox.Style = BoxStyle.Shape;
        {
        ModelStencil stencil;
        if (config.modelStencils.TryGetValue(graphicItem.Model, out stencil))
          modelBox.Shape = stencil.ShapeTemplate(graphicItem.MirrorX, graphicItem.MirrorY);
        else
          modelBox.Shape = ShapeTemplate.FromId("Decision2");

        AnchorPointCollection anchorPointCollection = new AnchorPointCollection();
        if (stencil.Anchors != null)
          {
          foreach (Anchor anchor in stencil.Anchors)
            {
            anchorPointCollection.Add(new AnchorPoint((short)anchor.position.X, (short)anchor.position.Y,
              (anchor.maxIn > 0), (anchor.maxOut > 0), MarkStyle.Circle, Color.Blue));
            }
          modelBox.AnchorPattern = new AnchorPattern(anchorPointCollection);
          }
        }

      modelBox.FillColor = Color.FromArgb(150, System.Drawing.Color.BurlyWood);
      modelBox.FrameColor = Color.FromArgb(200, System.Drawing.Color.BurlyWood);
      modelBox.Visible = ShowModels && isVisible;

      graphicBox.BoundingRect = new RectangleF(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
      graphicBox.RotationAngle = graphicItem.Angle;
      graphicBox.Text = graphicItem.Tag;
      graphicBox.ToolTip = graphicItem.Tag + "\n\nClassID: " + graphicItem.Model;
      graphicBox.Style = BoxStyle.Shape;
        {
        GraphicStencil stencil;
        if (config.graphicStencils.TryGetValue(graphicItem.Shape, out stencil))
          graphicBox.Shape = stencil.ShapeTemplate(graphicItem.MirrorX, graphicItem.MirrorY);
        else
          graphicBox.Shape = ShapeTemplate.FromId("Decision2");
        }
      graphicBox.EnabledHandles = Handles.None;
      graphicBox.HandlesStyle = HandlesStyle.Invisible;
      graphicBox.Visible = ShowGraphics && isVisible;

      graphicBox.ZBottom();
      modelBox.ZTop();

      justCreatedModelBox = modelBox;

      Item item = new Item(graphicItem.Guid, graphicItem.Tag, modelBox, graphicBox, isVisible);

      modelBox.Tag = item;
      graphicBox.Tag = item;

      items.Add(item.Guid, item);
      }

    internal void ItemVisible(Guid guid, bool visible)
      {
      Item item;
      if (items.TryGetValue(guid, out item))
        {
        item.Visible = visible;
        item.Model.Visible = visible && (item.Model.Selected || ShowModels);
        item.Graphic.Visible = visible && ShowGraphics;

        foreach (Arrow arrowDestination in item.Model.IncomingArrows)
          {
          arrowDestination.Visible = visible && ShowLinks;
          }

        foreach (Arrow arrowOrigin in item.Model.OutgoingArrows)
          {
          arrowOrigin.Visible = visible && ShowLinks;
          }
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

      return ((link != null) || (item != null));
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

      return false;
      }

    internal void SetStencil(Guid guid, ShapeTemplate shapeTemplate)
      {
      Item item;
      items.TryGetValue(guid, out item);
      if (item != null)
        item.Graphic.Shape = shapeTemplate;
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
            item.Graphic.Shape = stencil.ShapeTemplate(graphicItem.MirrorX, graphicItem.MirrorY);
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
        // Unimplemented...
        }
      }

    internal ModelStencil ModelStencil(string stencilName)
      {
      ModelStencil modelStencil;
      config.modelStencils.TryGetValue(stencilName, out modelStencil);
      return modelStencil;
      }

    internal GraphicStencil GraphicStencil(string stencilName)
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
      GraphicItem graphicItem;
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
      GraphicLink graphicLink;
      graphic.graphicLinks.TryGetValue((arrow.Tag as Link).Guid, out graphicLink);
      return graphicLink;
      }

    internal IEnumerable<GraphicItem> GraphicItems
      {
      get { return graphic.graphicItems.Values; }
      }

    internal IEnumerable<GraphicLink> GraphicLinks
      {
      get { return graphic.graphicLinks.Values; }
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
      get { return tvNavigation.SelectedNode.Key; }
      }


    internal bool IsItem(Guid guid)
      {
      return graphic.graphicItems.ContainsKey(guid);
      }

    internal void ModifyGraphicItem(Guid guid, RectangleF boundingRect, float angle)
      {
      graphic.ModifyItem(guid, boundingRect, angle);
      }

    internal GraphicItem NewGraphicItem(Guid guid, String tag, String area)
      {
      GraphicItem graphicItem = new GraphicItem(guid, tag);
      tvNavigation.GetNodeByKey(area).Nodes.Add(tag, guid.ToString());
      return NewGraphicItem(guid, area, graphicItem);
      }

    internal GraphicItem NewGraphicItem(String tag, String area)
      {
      Guid guid = Guid.NewGuid();
      GraphicItem graphicItem = new GraphicItem(guid, tag);
      tvNavigation.GetNodeByKey(area).Nodes.Add(tag, guid.ToString());
      return NewGraphicItem(guid, area, graphicItem);
      }

    internal GraphicItem NewGraphicItem(Guid guid, string area, GraphicItem graphicItem)
      {
      graphic.graphicItems.Add(guid, graphicItem);
      return graphicItem;
      }

    internal void ConnectGraphicItemModified(Graphic.ItemModifiedHandler itemModifiedHandler)
      {
      graphic.ItemModified += itemModifiedHandler;
      }

    internal void DisconnectGraphicItemModified(Graphic.ItemModifiedHandler itemModifiedHandler)
      {
      graphic.ItemModified -= itemModifiedHandler;
      }
    }
  }
