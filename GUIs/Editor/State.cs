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
    private Arrow arrow;

    private GraphicLink graphicLink;

    private bool visible;

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
  }

  public class Item
  {
    private Box model;
    private Box graphic;
    private PureComponents.TreeView.Node node;

    private GraphicItem graphicItem;

    private bool visible;
    private bool selected;

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

    public Item(Box model, Box graphic, bool visible)
    {
      this.model = model;
      this.graphic = graphic;
      this.visible = visible;
    }
  }

  //---------------------------------------------------------

  public class State
  {
    private Dictionary<string, Link> links = new Dictionary<string, Link>();
    private Dictionary<string, Item> items = new Dictionary<string, Item>();

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

    public Arrow Arrow(string tag)
    {
      Link link;
      links.TryGetValue(tag, out link);
      return link.Arrow;
    }

    public Item item(string tag)
    {
      Item item;
      items.TryGetValue(tag, out item);
      return item;
    }

    public void setArrow(string tag, Arrow arrow)
    {
      Link link = new Link();
      link.Arrow = arrow;
      links.Add(tag, link);
    }

    internal void newGraphicLink(SysCAD.Interface.GraphicLink graphicLink, Arrow arrow, bool visible)
    {
      Item origin = null; 
      Item destination = null;

      if (graphicLink.Source != null) origin = item(graphicLink.Source);
      if (graphicLink.Destination != null) destination = item(graphicLink.Destination);

      PointF pointOrigin = new PointF();
      PointF pointDestination = new PointF();

      if (graphicLink.controlPoints.Count > 1)
      {
        pointOrigin = graphicLink.controlPoints[0];
        pointDestination = graphicLink.controlPoints[graphicLink.controlPoints.Count - 1];
      }

      if (origin != null) 
        arrow.Origin = origin.Model;
      if (destination != null)
        arrow.Destination = destination.Model;

      arrow.Text = graphicLink.Tag;
      arrow.ToolTip = "Tag: " + graphicLink.Tag + "\n\nOrigin: " + graphicLink.Source + "\nDestination: " + graphicLink.Destination;
      arrow.ArrowHead = ArrowHead.Triangle;
      arrow.Style = ArrowStyle.Cascading;

      if (graphicLink.controlPoints.Count > 1)
      {
        arrow.SegmentCount = (short)(graphicLink.controlPoints.Count-1);
        int i = 0;
        foreach (PointF point in graphicLink.controlPoints)
          arrow.ControlPoints[i++] = point;
        arrow.UpdateFromPoints();
        PointCollection a = arrow.ControlPoints;
      }

      Link link = new Link();
      link.Arrow = arrow;
      link.Visible = true;

      arrow.Tag = link;

      arrow.Visible = ShowLinks && visible;

      links.Add(graphicLink.Tag, link);
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

    internal void newItem(SysCAD.Interface.GraphicItem graphicItem, bool isVisible, FlowChart flowchart)
    {
      Box modelBox = flowchart.CreateBox(0.0F, 0.0F, 10.0F, 10.0F);
      Box graphicBox = flowchart.CreateBox(0.0F, 0.0F, 10.0F, 10.0F);

      modelBox.BoundingRect = new RectangleF(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
      modelBox.RotationAngle = graphicItem.Angle;
      modelBox.Text = graphicItem.Tag;
      modelBox.ToolTip = "Tag: " + graphicItem.Tag + "\n\nModel Type: " + graphicItem.Model;
      modelBox.Style = BoxStyle.Shape;
      {
        ModelStencil stencil;
        if (config.modelStencils.TryGetValue(graphicItem.Model, out stencil))
          modelBox.Shape = stencil.ShapeTemplate(graphicItem.MirrorX, graphicItem.MirrorY);
        else
          modelBox.Shape = ShapeTemplate.FromId("Decision2");
      }
      modelBox.AnchorPattern = new AnchorPattern(new AnchorPoint[]
        {
          new AnchorPoint(50, 0, false, true, MarkStyle.Circle, Color.Blue),
          new AnchorPoint(0, 50, true, false, MarkStyle.Circle, Color.Blue),
          new AnchorPoint(50, 100, true, false, MarkStyle.Rectangle, Color.Green),
          new AnchorPoint(100, 50, true, true, MarkStyle.Rectangle, Color.Red)
        });

      modelBox.FillColor = Color.FromArgb(150, System.Drawing.Color.BurlyWood);
      modelBox.FrameColor = Color.FromArgb(200, System.Drawing.Color.BurlyWood);
      modelBox.Visible = ShowModels && isVisible;

      graphicBox.BoundingRect = new RectangleF(graphicItem.X, graphicItem.Y, graphicItem.Width, graphicItem.Height);
      graphicBox.RotationAngle = graphicItem.Angle;
      graphicBox.Text = graphicItem.Tag;
      graphicBox.ToolTip = "Tag: " + graphicItem.Tag + "\n\nStencil: " + graphicItem.Shape; ;
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

      Item item = new Item(modelBox, graphicBox, isVisible);

      modelBox.Tag = item;
      graphicBox.Tag = item;

      items.Add(graphicItem.Tag, item);
    }

    internal void ItemVisible(string tag, bool visible)
    {
      Item item;
      if (items.TryGetValue(tag, out item))
      {
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

    internal void ItemSelected(string tag, bool selected)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
      {
        item.Selected = selected;
        item.Model.Selected = selected && item.Visible;
      }
    }

    internal bool Exists(string tag)
    {
      Item item;
      items.TryGetValue(tag, out item);

      Link link;
      links.TryGetValue(tag, out link);

      return ((link != null) || (item != null));
    }

    internal void SetStencil(string tag, ShapeTemplate shapeTemplate)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
        item.Graphic.Shape = shapeTemplate;
    }

    internal void SetAngle(string tag, float angle)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
      {
        item.Model.RotationAngle = angle;
        item.Graphic.RotationAngle = angle;
      }
    }

    internal void SetHeight(string tag, float height)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Height = height;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetWidth(string tag, float width)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Width = width;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetX(string tag, float x)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.X = x;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetY(string tag, float y)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
      {
        RectangleF boundingRect = item.Model.BoundingRect;
        boundingRect.Y = y;
        item.Model.BoundingRect = boundingRect;
        item.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetMirrorX(string tag, bool mirrorX)
    {
      Item item;
      items.TryGetValue(tag, out item);
      if (item != null)
      {
        GraphicItem graphicItem;
        if (graphic.graphicItems.TryGetValue(tag, out graphicItem))
        {
          GraphicStencil stencil;
          if (config.graphicStencils.TryGetValue(graphicItem.Shape, out stencil))
          {
            item.Graphic.Shape = stencil.ShapeTemplate(graphicItem.MirrorX, graphicItem.MirrorY);
          }
        }
      }
    }

    internal void SetMirrorY(string tag, bool mirrorY)
    {
      Item item;
      items.TryGetValue(tag, out item);
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

    internal GraphicItem GraphicItem(string tag)
    {
      GraphicItem graphicItem;
      graphic.graphicItems.TryGetValue(tag, out graphicItem);
      return graphicItem;
    }

    internal GraphicLink GraphicLink(string tag)
    {
      GraphicLink graphicLink;
      graphic.graphicLinks.TryGetValue(tag, out graphicLink);
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

    internal IEnumerable<GraphicArea> GraphicAreas
    {
      get { return graphic.___graphicAreas.Values; }
    }

    internal Item Item(string tag)
    {
      Item item;
      items.TryGetValue(tag, out item);
      return item;
    }

    internal Link Link(string tag)
    {
      Link link;
      links.TryGetValue(tag, out link);
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

    internal string CurrentArea
    {
      get {return tvNavigation.SelectedNode.Text; }
    }

  
  internal bool IsItem(string tag)
    {
      return graphic.graphicItems.ContainsKey(tag);
    }

    internal void ModifyGraphicItem(string tag, RectangleF boundingRect, float angle)
    {
      graphic.ModifyItem(tag, boundingRect, angle);
    }

    internal GraphicItem NewGraphicItem(string tag, string area)
    {
      GraphicItem graphicItem = new GraphicItem(tag);
      tvNavigation.GetNodeByKey(area).Nodes.Add(tag, tag);
      return NewGraphicItem(tag, area, graphicItem);
    }

    internal GraphicItem NewGraphicItem(string tag, string area, GraphicItem graphicItem)
    {
      graphic.graphicItems.Add(tag, graphicItem);
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
