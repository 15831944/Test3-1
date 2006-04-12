using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Interface;

namespace SysCAD.Editor
{
  public class BODLink
  {
    private Arrow arrow;
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

  public class BODThing
  {
    private Box model;
    private Box graphic;
    private PureComponents.TreeView.Node node;

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

    public BODThing(Box model, Box graphic, bool visible)
    {
      this.model = model;
      this.graphic = graphic;
      this.visible = visible;
    }
  }

  //---------------------------------------------------------

  public class BOD
  {
    public Dictionary<string, BODLink> links = new Dictionary<string, BODLink>();
    public Dictionary<string, BODThing> things = new Dictionary<string, BODThing>();

    public Graphic graphic;
    public Config config;

    public bool ShowModels = false;
    public bool ShowGraphics = true;
    public bool ShowLinks = true;
    public bool ShowTags = true;

    public bool SelectLinks = true;
    public bool SelectItems = true;

    private Box justCreatedModelBox;

    public Arrow Arrow(string tag)
    {
      BODLink link;
      links.TryGetValue(tag, out link);
      return link.Arrow;
    }

    public BODThing Thing(string tag)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      return thing;
    }

    public void setArrow(string tag, Arrow arrow)
    {
      BODLink link = new BODLink();
      link.Arrow = arrow;
      links.Add(tag, link);
    }

    internal void newLink(SysCAD.Interface.Link link, Arrow arrow, bool visible)
    {
      BODThing origin = null; 
      BODThing destination = null;

      if (link.Origin != null) origin = Thing(link.Origin);
      if (link.Destination != null) destination = Thing(link.Destination);

      PointF pointOrigin = new PointF();
      PointF pointDestination = new PointF();

      if (link.controlPoints.Count > 1)
      {
        pointOrigin = link.controlPoints[0];
        pointDestination = link.controlPoints[link.controlPoints.Count - 1];
      }

      if (origin != null) 
        arrow.Origin = origin.Model;
      if (destination != null)
        arrow.Destination = destination.Model;

      arrow.Text = link.Tag;
      arrow.ToolTip = "Tag: " + link.Tag + "\n\nOrigin: " + link.Origin + "\nDestination: " + link.Destination;
      arrow.ArrowHead = ArrowHead.Triangle;
      arrow.Style = ArrowStyle.Cascading;

      if (link.controlPoints.Count > 1)
      {
        arrow.SegmentCount = (short)(link.controlPoints.Count-1);
        int i = 0;
        foreach (PointF point in link.controlPoints)
          arrow.ControlPoints[i++] = point;
        arrow.UpdateFromPoints();
        PointCollection a = arrow.ControlPoints;
      }

      BODLink bodLink = new BODLink();
      bodLink.Arrow = arrow;
      bodLink.Visible = true;

      arrow.Tag = bodLink;

      arrow.Visible = ShowLinks && visible;

      links.Add(link.Tag, bodLink);
    }

    internal void Remove(BODThing thing, FlowChart flowchart)
    {
      if (thing != null)
      {
        if (thing.Model != null)
          flowchart.DeleteObject(thing.Model);
        if (thing.Graphic != null)
          flowchart.DeleteObject(thing.Graphic);
        if (thing.Node != null)
          thing.Node.Remove();
      }
    }

    internal void newThing(SysCAD.Interface.Item item, Box modelBox, Box graphicBox, bool isVisible, FlowChart flowchart)
    {
      modelBox.BoundingRect = new RectangleF(item.X, item.Y, item.Width, item.Height);
      modelBox.RotationAngle = item.Angle;
      modelBox.Text = item.Tag;
      modelBox.ToolTip = "Tag: " + item.Tag + "\n\nModel Type: " + item.Model;
      modelBox.Style = BoxStyle.Shape;
      {
        ModelStencil stencil;
        if (config.modelStencils.TryGetValue(item.Model, out stencil))
          modelBox.Shape = stencil.ShapeTemplate(item.MirrorX, item.MirrorY);
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

      graphicBox.BoundingRect = new RectangleF(item.X, item.Y, item.Width, item.Height);
      graphicBox.RotationAngle = item.Angle;
      graphicBox.Text = item.Tag;
      graphicBox.ToolTip = "Tag: " + item.Tag + "\n\nStencil: " + item.Shape; ;
      graphicBox.Style = BoxStyle.Shape;
      {
        GraphicStencil stencil;
        if (config.graphicStencils.TryGetValue(item.Shape, out stencil))
          graphicBox.Shape = stencil.ShapeTemplate(item.MirrorX, item.MirrorY);
        else
          graphicBox.Shape = ShapeTemplate.FromId("Decision2");
      }
      graphicBox.AttachTo(modelBox, 0, 0, 100, 100);
      graphicBox.EnabledHandles = Handles.None;
      graphicBox.HandlesStyle = HandlesStyle.Invisible;
      graphicBox.Visible = ShowGraphics && isVisible;

      graphicBox.ZBottom();
      modelBox.ZTop();

      justCreatedModelBox = modelBox;

      BODThing thing = new BODThing(modelBox, graphicBox, isVisible);

      modelBox.Tag = thing;
      graphicBox.Tag = thing;

      things.Add(item.Tag, thing);
    }

    internal void ThingVisible(string tag, bool visible)
    {
      BODThing thing;
      if (things.TryGetValue(tag, out thing))
      {
        thing.Model.Visible = visible && (thing.Model.Selected || ShowModels);
        thing.Graphic.Visible = visible && ShowGraphics;

        foreach (Arrow arrowDestination in thing.Model.IncomingArrows)
        {
          arrowDestination.Visible = visible && ShowLinks;
        }

        foreach (Arrow arrowOrigin in thing.Model.OutgoingArrows)
        {
          arrowOrigin.Visible = visible && ShowLinks;
        }
      }
    }

    internal void ThingSelected(string tag, bool selected)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        thing.Selected = selected;
        thing.Model.Selected = selected && thing.Visible;
      }
    }

    internal bool Exists(string tag)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);

      BODLink link;
      links.TryGetValue(tag, out link);

      return ((link != null) || (thing != null));
    }

    internal void SetStencil(string tag, ShapeTemplate shapeTemplate)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
        thing.Graphic.Shape = shapeTemplate;
    }

    internal void SetAngle(string tag, float angle)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        thing.Model.RotationAngle = angle;
        thing.Graphic.RotationAngle = angle;
      }
    }

    internal void SetHeight(string tag, float height)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        RectangleF boundingRect = thing.Model.BoundingRect;
        boundingRect.Height = height;
        thing.Model.BoundingRect = boundingRect;
        thing.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetWidth(string tag, float width)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        RectangleF boundingRect = thing.Model.BoundingRect;
        boundingRect.Width = width;
        thing.Model.BoundingRect = boundingRect;
        thing.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetX(string tag, float x)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        RectangleF boundingRect = thing.Model.BoundingRect;
        boundingRect.X = x;
        thing.Model.BoundingRect = boundingRect;
        thing.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetY(string tag, float y)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        RectangleF boundingRect = thing.Model.BoundingRect;
        boundingRect.Y = y;
        thing.Model.BoundingRect = boundingRect;
        thing.Graphic.BoundingRect = boundingRect;
      }
    }

    internal void SetMirrorX(string tag, bool mirrorX)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        Item item;
        if (graphic.items.TryGetValue(tag, out item))
        {
          GraphicStencil stencil;
          if (config.graphicStencils.TryGetValue(item.Shape, out stencil))
          {
            thing.Graphic.Shape = stencil.ShapeTemplate(item.MirrorX, item.MirrorY);
          }
        }
      }
    }

    internal void SetMirrorY(string tag, bool mirrorY)
    {
      BODThing thing;
      things.TryGetValue(tag, out thing);
      if (thing != null)
      {
        // Unimplemented...
      }
    }
  }
}
