using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using MindFusion.FlowChartX;

using SysCAD.Interface;

namespace SysCAD.Editor
{
  public partial class FrmFlowChart : Form
  {
    public Dictionary<string, ItemBox> itemBoxes;

    public Graphic graphic;
    public Config config;
    private PureComponents.TreeView.TreeView tvNavigation;

    private Box justCreatedModelBox;

    public bool ShowModels = false;
    public bool ShowGraphics = true;
    public bool ShowLinks = true;
    public bool ShowTags = true;

    public bool SelectLinks = true;
    public bool SelectItems = true;

    public string currentModelShape;
    public string currentGraphicShape;

    private int tempBoxKey = 0;
    private int tempArrowKey = 0;

    public FrmFlowChart()
    {
      InitializeComponent();

      itemBoxes = new Dictionary<string, ItemBox>();

      fcFlowChart.DocExtents = new RectangleF(0.0F, 0.0F, 297.0F, 210.0F);
      fcFlowChart.Selection.Style = SelectionStyle.SemiTransparent;
    }

    internal void SetProject(Graphic graphic, Config config, PureComponents.TreeView.TreeView tvNavigation)
    {
      this.graphic = graphic;
      //graphic.ItemModified += new Graphic.ItemModifiedHandler(graphic_ItemModified);

      this.config = config;
      this.tvNavigation = tvNavigation;

      SuspendLayout();
      fcFlowChart.UndoManager.UndoEnabled = false;
      fcFlowChart.UseWaitCursor = true;
      fcFlowChart.ValidityChecks = false;
      fcFlowChart.Visible = false;
      fcFlowChart.Enabled = false;

      foreach (Item item in graphic.items.Values)
      {
        NewGraphicItem(item, null, false);
      }

      foreach (Link link in graphic.links.Values)
      {
        NewGraphicLink(link, null, false);
      }

      fcFlowChart.UndoManager.UndoEnabled = true;
      fcFlowChart.UseWaitCursor = false;
      fcFlowChart.ValidityChecks = true;
      fcFlowChart.Visible = true;
      fcFlowChart.Enabled = true;

      fcFlowChart.RouteAllArrows();

      ResumeLayout(false);

      ZoomToVisible();
    }

    private void NewGraphicLink(Link link, Arrow arrow, bool isVisible)
    {
      ItemBox itemBoxOrigin;
      ItemBox itemBoxDestination;

      Box boxOrigin = new Box(fcFlowChart);
      Box boxDestination = new Box(fcFlowChart);

      if (link.Source != null)
      {
        itemBoxes.TryGetValue(link.Source, out itemBoxOrigin);
        ItemBox itemBox;
        itemBoxes.TryGetValue(link.Source, out itemBox);
        if (itemBox != null)
          boxOrigin = itemBox.ModelBox;
        else
          boxOrigin = null;
      }
      if (link.Destination != null)
      {
        itemBoxes.TryGetValue(link.Destination, out itemBoxDestination);
        ItemBox itemBox;
        itemBoxes.TryGetValue(link.Destination, out itemBox);
        if (itemBox != null)
          boxDestination = itemBox.ModelBox;
        else
          boxDestination = null;
      }

      PointF pointOrigin = new PointF();
      PointF pointDestination = new PointF();

      if (link.controlPoints.Count > 1)
      {
        pointOrigin = link.controlPoints[0];
        pointDestination = link.controlPoints[link.controlPoints.Count - 1];
      }

      if (arrow == null)
        arrow = fcFlowChart.CreateArrow(boxOrigin, boxDestination);

      if (arrow != null)
      {
        arrow.Text = link.Tag;
        arrow.ToolTip = "Tag: " + link.Tag + "\n\nOrigin: " + link.Source + "\nDestination: " + link.Destination;
        arrow.ArrowHead = ArrowHead.Triangle;
        arrow.Style = ArrowStyle.Cascading;

        if (link.controlPoints.Count > 0)
        {
          arrow.ControlPoints.Clear();
          foreach (PointF point in link.controlPoints)
            arrow.ControlPoints.Add(point);
          arrow.UpdateFromPoints();
        }

        arrow.Visible = ShowLinks && isVisible;
      }
    }

    private void NewGraphicItem(Item item, Box graphicBox, bool isVisible)
    {
      Box modelBox;

      modelBox = fcFlowChart.CreateBox(item.X + item.Width * 0.25F, item.Y + item.Height * 0.25F, item.Width * 0.5F, item.Height * 0.5F);
      modelBox.Text = item.Tag;
      modelBox.ToolTip = "Tag: " + item.Tag + "\n\nModel Type: " + item.Model;
      modelBox.Style = BoxStyle.Shape;
      {
        ModelStencil stencil;
        if (config.modelStencils.TryGetValue(item.Model, out stencil))
          modelBox.Shape = stencil.ShapeTemplate();
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

      modelBox.FillColor = System.Drawing.Color.BurlyWood;
      modelBox.FrameColor = System.Drawing.Color.BurlyWood;
      modelBox.Visible = ShowModels && isVisible;

      if (graphicBox == null)
        graphicBox = fcFlowChart.CreateBox(item.X, item.Y, item.Width, item.Height);
      graphicBox.Text = item.Tag;
      graphicBox.ToolTip = "Tag: " + item.Tag + "\n\nStencil: " + item.Shape; ;
      graphicBox.Style = BoxStyle.Shape;
      {
        GraphicStencil stencil;
        if (config.graphicStencils.TryGetValue(item.Shape, out stencil))
          graphicBox.Shape = stencil.ShapeTemplate();
        else
          graphicBox.Shape = ShapeTemplate.FromId("Decision2");
      }
      //GraphicStencil tempGraphicStencil;
      //if (config.graphicStencils.TryGetValue(item.shape, out tempGraphicStencil))
      //  graphicBox.Shape = tempGraphicStencil.ShapeTemplate();
      graphicBox.AttachTo(modelBox, -50, -50, 150, 150);
      graphicBox.EnabledHandles = Handles.None;
      graphicBox.HandlesStyle = HandlesStyle.Invisible;
      graphicBox.Visible = ShowGraphics && isVisible;

      graphicBox.ZBottom();
      modelBox.ZTop();

      justCreatedModelBox = modelBox;

      itemBoxes.Add(item.Tag, new ItemBox(modelBox, graphicBox, false));
    }

    public void ZoomToVisible()
    {
      float minX =  Single.MaxValue, minY =  Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

      bool foundVisibleObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {
        if (box.Visible)
        {
          foundVisibleObject = true;
          if (box.BoundingRect.Left < minX)   minX = box.BoundingRect.Left;
          if (box.BoundingRect.Top < minY)    minY = box.BoundingRect.Top;
          if (box.BoundingRect.Right > maxX)  maxX = box.BoundingRect.Right;
          if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {
        if (arrow.Visible)
        {
          foundVisibleObject = true;
          if (arrow.BoundingRect.Left < minX)   minX = arrow.BoundingRect.Left;
          if (arrow.BoundingRect.Top < minY)    minY = arrow.BoundingRect.Top;
          if (arrow.BoundingRect.Right > maxX)  maxX = arrow.BoundingRect.Right;
          if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
        }
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundVisibleObject)
        fcFlowChart.ZoomToRect(new RectangleF(minX - width * 0.05F, minY - height * 0.05F, width * 1.1F, height * 1.1F));
      else
        fcFlowChart.ZoomToRect(new RectangleF(fcFlowChart.DocExtents.Left, fcFlowChart.DocExtents.Top, fcFlowChart.DocExtents.Width, fcFlowChart.DocExtents.Height));
    }

    public void ZoomToSelected()
    {
      float minX = Single.MaxValue, minY = Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

      bool foundSelectedObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {
        if (box.Selected)
        {
          foundSelectedObject = true;
          if (box.BoundingRect.Left < minX)   minX = box.BoundingRect.Left;
          if (box.BoundingRect.Top < minY)    minY = box.BoundingRect.Top;
          if (box.BoundingRect.Right > maxX)  maxX = box.BoundingRect.Right;
          if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {
        if (arrow.Selected)
        {
          foundSelectedObject = true;
          if (arrow.BoundingRect.Left < minX)   minX = arrow.BoundingRect.Left;
          if (arrow.BoundingRect.Top < minY)    minY = arrow.BoundingRect.Top;
          if (arrow.BoundingRect.Right > maxX)  maxX = arrow.BoundingRect.Right;
          if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
        }
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundSelectedObject)
        fcFlowChart.ZoomToRect(new RectangleF(minX - width * 0.05F, minY - height * 0.05F, width * 1.1F, height * 1.1F));
      else
        fcFlowChart.ZoomToRect(new RectangleF(fcFlowChart.DocExtents.Left, fcFlowChart.DocExtents.Top, fcFlowChart.DocExtents.Width, fcFlowChart.DocExtents.Height));
    }

    internal void SetVisible(string tag, bool visible)
    {
      itemBoxes[tag].Visible = visible;
      itemBoxes[tag].ModelBox.Visible = visible && (itemBoxes[tag].ModelBox.Selected || ShowModels);
      itemBoxes[tag].GraphicBox.Visible = visible && ShowGraphics;

      foreach (Arrow arrowDestination in itemBoxes[tag].ModelBox.IncomingArrows)
      {
        arrowDestination.Visible = visible && ShowLinks;
      }

      foreach (Arrow arrowOrigin in itemBoxes[tag].ModelBox.OutgoingArrows)
      {
        arrowOrigin.Visible = visible && ShowLinks;
      }
    }

    internal void SetSelected(string tag, bool selected)
    {
      itemBoxes[tag].ModelBox.Selected = selected && (itemBoxes[tag].ModelBox.Visible || itemBoxes[tag].GraphicBox.Visible);
    }

    private void fcFlowChart_ArrowAttaching(object sender, AttachConfirmArgs e)
    {
      if (e.ChangingOrigin) // Origin is trying to be being changed.
      {
        if (e.Origin==null)
          e.Confirm = false; // Trying to be disconnected, ignore.
        else
          e.Confirm = true; // Being attached to another node, OK.
      }
      else // Destination is trying to be being changed.
      {
        if (e.Destination==null)
          e.Confirm = false; // Trying to be disconnected, ignore.
        else
          e.Confirm = true; // Being attached to another node, OK.
      }
    }

    Node savedOrigin;
    Node savedDestination;

    int savedOriginAnchor;
    int savedDestinationAnchor;

    private void fcFlowChart_ArrowModified(object sender, ArrowMouseArgs e)
    {
      if (fcFlowChart.Selection.Arrows.Count == 1) // We're playing with just one arrow...
      {
        PointCollection controlPoints = e.Arrow.ControlPoints.Clone();

        if (savedOrigin != null)
        {
          e.Arrow.Origin = savedOrigin;
          e.Arrow.OrgnAnchor = savedOriginAnchor;
        }

        if (savedDestination != null)
        {
          e.Arrow.Destination = savedDestination;
          e.Arrow.DestAnchor = savedDestinationAnchor;
        }

        e.Arrow.ControlPoints.Clear();
        foreach (PointF point in controlPoints)
        {
          e.Arrow.ControlPoints.Add(point);
        }
        e.Arrow.UpdateFromPoints();
      }
    }

    private void fcFlowChart_ArrowModifying(object sender, ArrowMouseArgs e)
    {
      if (fcFlowChart.Selection.Arrows.Count == 1) // We're playing with just one arrow...
      {
        savedOrigin = null;
        savedDestination = null;

        Box originBox = (e.Arrow.Origin as Box);
        if (originBox != null)
        {
          ItemBox originItemBox = itemBoxes[originBox.Text];
          if (originItemBox != null)
          {
            savedOrigin = originItemBox.ModelBox;
            savedOriginAnchor = e.Arrow.OrgnAnchor;
          }
        }

        Box destinationBox = (e.Arrow.Destination as Box);
        if (destinationBox != null)
        {
          ItemBox destinationItemBox = itemBoxes[destinationBox.Text];
          if (destinationItemBox != null)
          {
            savedDestination = destinationItemBox.ModelBox;
            savedDestinationAnchor = e.Arrow.DestAnchor;
          }
        }
      }
    }

    private ItemBox oldHoverItemBox = null;
    private Arrow oldHoverArrow = null;

    public void fcFlowChart_MouseMove(object sender, MouseEventArgs e)
    {
      SuspendLayout();

      Box hoverBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)));
      ItemBox hoverItemBox = null;
      if (hoverBox != null)
        hoverItemBox = itemBoxes[hoverBox.Text];

      Arrow hoverArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 1);


      if (oldHoverItemBox != null) // deal with old itemBox.
      {
        if (oldHoverItemBox == hoverItemBox) // nothings changed.
          return;
        else // we've moved on, un-hover the old one.
        {
          oldHoverItemBox.GraphicBox.ZBottom();
          oldHoverItemBox.GraphicBox.Visible = ShowGraphics;
          oldHoverItemBox.ModelBox.ZTop();
          oldHoverItemBox.ModelBox.Visible = oldHoverItemBox.ModelBox.Selected || ShowModels;

          foreach (Arrow arrow in oldHoverItemBox.IncomingArrows)
          {
            arrow.ZTop();
            arrow.Visible = ShowLinks;
          }

          foreach (Arrow arrow in oldHoverItemBox.OutgoingArrows)
          {
            arrow.ZTop();
            arrow.Visible = ShowLinks;
          }
        }
      }

      if (oldHoverArrow != null) // deal with old arrow.
      {
        if (oldHoverArrow == hoverArrow) // nothings changed...
          return;
        else
        {
          Box originBox = (oldHoverArrow.Origin as Box);
          if (originBox != null)
          {
            ItemBox originItemBox = itemBoxes[originBox.Text];
            if (originItemBox != null)
            {
              originItemBox.GraphicBox.ZBottom();
              originItemBox.GraphicBox.Visible = ShowGraphics;
              originItemBox.ModelBox.ZTop();
              originItemBox.ModelBox.Visible = originItemBox.ModelBox.Selected || ShowModels;
            }
          }

          Box destinationBox = (oldHoverArrow.Destination as Box);
          if (destinationBox != null)
          {
            ItemBox destinationItemBox = itemBoxes[destinationBox.Text];
            if (destinationItemBox != null)
            {
              destinationItemBox.GraphicBox.ZBottom();
              destinationItemBox.GraphicBox.Visible = ShowGraphics;
              destinationItemBox.ModelBox.ZTop();
              destinationItemBox.ModelBox.Visible = destinationItemBox.ModelBox.Selected || ShowModels;
            }
          }
        }

        oldHoverArrow.ZTop();
        oldHoverArrow.Visible = ShowLinks;
      }

      if (hoverItemBox != null)
      {
        hoverItemBox.GraphicBox.ZBottom();
        hoverItemBox.GraphicBox.Visible = true;
        hoverItemBox.ModelBox.ZTop();
        hoverItemBox.ModelBox.Visible = true;

        foreach (Arrow arrow in hoverItemBox.IncomingArrows)
        {
          arrow.ZTop();
          arrow.Visible = true;
        }

        foreach (Arrow arrow in hoverItemBox.OutgoingArrows)
        {
          arrow.ZTop();
          arrow.Visible = true;
        }
      }
      else if (hoverArrow != null)
      {
        Box originBox = (hoverArrow.Origin as Box);
        if (originBox != null)
        {
          ItemBox originItemBox = itemBoxes[originBox.Text];
          if (originItemBox != null)
          {
            originItemBox.GraphicBox.ZBottom();
            originItemBox.GraphicBox.Visible = true;
            originItemBox.ModelBox.ZTop();
            originItemBox.ModelBox.Visible = true;
          }
        }

        Box destinationBox = (hoverArrow.Destination as Box);
        if (destinationBox != null)
        {
          ItemBox destinationItemBox = itemBoxes[destinationBox.Text];
          if (destinationItemBox != null)
          {
            destinationItemBox.GraphicBox.ZBottom();
            destinationItemBox.GraphicBox.Visible = true;
            destinationItemBox.ModelBox.ZTop();
            destinationItemBox.ModelBox.Visible = true;
          }
        }

        hoverArrow.ZTop();
        hoverArrow.Visible = true;
      }

      oldHoverItemBox = hoverItemBox;
      oldHoverArrow = hoverArrow;

      ResumeLayout(false);
    }

    private void fcFlowChart_DrawBox(object sender, BoxDrawArgs e)
    {
      foreach (Arrow arrow in e.Box.IncomingArrows)
      {
        float dx = 0.5F;
        float dy = 0.5F;

        if (arrow.DestAnchor != -1)
        {
          dx = e.Box.AnchorPattern.Points[arrow.DestAnchor].X / 100.0F;
          dy = e.Box.AnchorPattern.Points[arrow.DestAnchor].Y / 100.0F;
        }

        PointF anchorPoint = new PointF(
          e.Box.BoundingRect.Left + e.Box.BoundingRect.Width * dx,
          e.Box.BoundingRect.Top + e.Box.BoundingRect.Height * dy);

        PointF[] extensionPoints =
          new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPoint };

        System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

        e.Graphics.DrawLines(pen, extensionPoints);
      }

      foreach (Arrow arrow in e.Box.OutgoingArrows)
      {
        float dx = 0.5F;
        float dy = 0.5F;

        if (arrow.OrgnAnchor != -1)
        {
          dx = e.Box.AnchorPattern.Points[arrow.OrgnAnchor].X / 100.0F;
          dy = e.Box.AnchorPattern.Points[arrow.OrgnAnchor].Y / 100.0F;
        }

        PointF anchorPoint = new PointF(
          e.Box.BoundingRect.Left + e.Box.BoundingRect.Width * dx,
          e.Box.BoundingRect.Top + e.Box.BoundingRect.Height * dy);

        PointF[] extensionPoints =
          new PointF[] { arrow.ControlPoints[0], anchorPoint };

        System.Drawing.Pen pen = new System.Drawing.Pen(Color.Blue, 0.0F);

        e.Graphics.DrawLines(pen, extensionPoints);
      }
    }

    private void fcFlowChart_BoxModified(object sender, BoxMouseArgs e)
    {
      Box graphicBox = itemBoxes[e.Box.Text].GraphicBox;
      graphic.ModifyItem(graphicBox.Text, graphicBox.BoundingRect);
    }

    private void graphic_ItemModified(string tag, RectangleF boundingRect)
    {
      Box graphicBox = itemBoxes[tag].GraphicBox;
      Box modelBox = itemBoxes[tag].ModelBox;

      graphicBox.BoundingRect = boundingRect;
      modelBox.BoundingRect = new RectangleF(boundingRect.X + boundingRect.Width * 0.25F, 
        boundingRect.Y + boundingRect.Height * 0.25F,
        boundingRect.Width * 0.5F, boundingRect.Height * 0.5F);

      graphicBox.ZBottom();
      modelBox.ZTop();

    }

    private void fcFlowChart_BoxCreated(object sender, BoxEventArgs e)
    {
      //NewItem(e.Box.BoundingRect, e.Box);
    }

    private void NewItem(RectangleF rect, Box box)
    {
      ItemBox tempItemBox;
      while (itemBoxes.TryGetValue("N_" + tempBoxKey.ToString(), out tempItemBox))
        tempBoxKey++;
      Item newItem = new Item("N_" + tempBoxKey.ToString());
      graphic.items.Add("N_" + tempBoxKey.ToString(), newItem);
      newItem.X = rect.X - rect.Width;
      newItem.Y = rect.Y - rect.Height;
      newItem.Width = rect.Width*2.0F;
      newItem.Height = rect.Height*2.0F;
      newItem.Model = currentModelShape;
      newItem.Shape = currentGraphicShape;

      tvNavigation.Nodes.Add("N_" + tempBoxKey.ToString(), "N_" + tempBoxKey.ToString());

      NewGraphicItem(newItem, box, true);

      tvNavigation.GetNodeByKey("N_" + tempBoxKey.ToString()).Checked = true;
    }

    private void fcFlowChart_Click(object sender, EventArgs e)
    {
      MouseEventArgs me = e as MouseEventArgs;
      Box overBox = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(me.Location));
      Arrow overArrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(me.Location), 1);

      if ((overBox == null) && (overArrow == null)) // we're in free space...
      {
        if (fcFlowChart.Behavior == BehaviorType.CreateBox)
        {
          NewItem(new RectangleF(fcFlowChart.ClientToDoc(me.Location), config.graphicStencils[currentGraphicShape].defaultSize), null);          
        }
      }
    }

    private void FrmFlowChart_Resize(object sender, EventArgs e)
    {
      SuspendLayout();
      fcFlowChart.Size = Size;
      fcFlowChart.Refresh();
      ResumeLayout();
    }

    private void fcFlowChart_BoxDeleted(object sender, BoxEventArgs e)
    {
      string removedTag = e.Box.Text;
      ItemBox removedItemBox;
      itemBoxes.TryGetValue(removedTag, out removedItemBox);

      if (removedItemBox != null)
      {
        if (removedItemBox.ModelBox != null) 
          fcFlowChart.DeleteObject(removedItemBox.ModelBox);
        if (removedItemBox.GraphicBox != null)
          fcFlowChart.DeleteObject(removedItemBox.GraphicBox);

        PureComponents.TreeView.Node removedNode = tvNavigation.GetNodeByKey(removedTag);
        if (removedNode != null)
          removedNode.Remove();
      }
    }

    private void fcFlowChart_ArrowDeleted(object sender, ArrowEventArgs e)
    {

    }

    private void fcFlowChart_ArrowCreated(object sender, ArrowEventArgs e)
    {
      tempArrowKey++;
      string newLinkTag = "A_" + tempArrowKey.ToString();
      e.Arrow.Text = newLinkTag;

      Box destinationBox = e.Arrow.Destination as Box;
      Box sourceBox = e.Arrow.Origin as Box;
      Link newLink = new Link(newLinkTag);

      if (destinationBox != null)
        newLink.Destination = destinationBox.Text;

      if (sourceBox != null)
        newLink.Source = sourceBox.Text;

      foreach (PointF point in e.Arrow.ControlPoints)
      {
        newLink.controlPoints.Add(point);
      }

      NewGraphicLink(newLink, e.Arrow, true);

      //fcFlowChart.DeleteObject(e.Arrow);
    }

    private void fcFlowChart_BoxCreating(object sender, BoxConfirmArgs e)
    {
      if (justCreatedModelBox != null)
        justCreatedModelBox.BoundingRect =
          new RectangleF(e.Box.BoundingRect.X + 0.25F * e.Box.BoundingRect.Width,
                         e.Box.BoundingRect.Y + 0.25F * e.Box.BoundingRect.Height,
                         0.5F * e.Box.BoundingRect.Height,
                         0.5F * e.Box.BoundingRect.Height);

      justCreatedModelBox.ZTop();

      e.Confirm = false;
    }
  }

  //---------------------------------------

  public class ItemBox
  {
    private Box modelBox;
    private Box graphicBox;
    private bool visible;

    public Box ModelBox
    {
      get { return modelBox; }
      set { modelBox = value; }
    }

    public Box GraphicBox
    {
      get { return graphicBox; }
      set { graphicBox = value; }
    }

    public bool Visible
    {
      get { return visible; }
      set { visible = value; }
    }

    public ArrowCollection IncomingArrows
    {
      get { return modelBox.IncomingArrows; }
    }

    public ArrowCollection OutgoingArrows
    {
      get { return modelBox.OutgoingArrows; }
    }

    public ItemBox(Box modelBox, Box graphicBox, bool visible)
    {
      this.modelBox = modelBox;
      this.graphicBox = graphicBox;
      this.visible = visible;
    }
  }

  //---------------------------------------------------------

}