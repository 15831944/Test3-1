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
  public struct ItemBox
  {
    public Box modelBox;
    public Box graphicBox;

    public ItemBox(Box modelBox, Box graphicBox)
    {
      this.modelBox = modelBox;
      this.graphicBox = graphicBox;
    }
  }

  //---------------------------------------------------------

  public partial class FrmFlowChart : Form
  {
    private Dictionary<string, ItemBox> itemBoxes;

    private Graphic graphic;
    private Config config;

    public FrmFlowChart()
    {
      InitializeComponent();

      itemBoxes = new Dictionary<string, ItemBox>();

      fcFlowChart.DocExtents = new RectangleF(0.0F, 0.0F, 297.0F, 210.0F);
      fcFlowChart.Selection.Style = SelectionStyle.SemiTransparent;
    }

    internal void SetProject(Graphic graphic, Config config)
    {
      this.graphic = graphic;
      graphic.ItemModified += new Graphic.ItemModifiedHandler(graphic_ItemModified);

      this.config = config;

      SuspendLayout();
      fcFlowChart.UndoManager.UndoEnabled = false;
      fcFlowChart.UseWaitCursor = true;
      fcFlowChart.ValidityChecks = false;
      fcFlowChart.Visible = false;
      fcFlowChart.Enabled = false;

      foreach (Item item in graphic.items.Values)
      {
        Box modelBox;
        Box graphicBox;

        modelBox = fcFlowChart.CreateBox(item.x + item.width * 0.25F, item.y + item.height * 0.25F, item.width * 0.5F, item.height * 0.5F);
        modelBox.Text = item.tag;
        modelBox.ToolTip = item.tag;
        modelBox.Style = BoxStyle.Shape;
        modelBox.Shape = ShapeTemplate.FromId("Decision2");
        modelBox.AnchorPattern = new AnchorPattern(new AnchorPoint[]
        {
          new AnchorPoint(50, 0, false, true, MarkStyle.Circle, Color.Blue),
          new AnchorPoint(0, 50, true, false, MarkStyle.Circle, Color.Blue),
          new AnchorPoint(50, 100, true, false, MarkStyle.Rectangle, Color.Green),
          new AnchorPoint(100, 50, true, true, MarkStyle.Rectangle, Color.Red)
        });

        modelBox.FillColor = System.Drawing.Color.BurlyWood;
        modelBox.FrameColor = System.Drawing.Color.BurlyWood;
        modelBox.Visible = false;
        modelBox.ZIndex = 1;

        graphicBox = fcFlowChart.CreateBox(item.x, item.y, item.width, item.height);
        graphicBox.Text = item.tag;
        graphicBox.ToolTip = item.tag;
        graphicBox.Style = BoxStyle.Shape;
        GraphicStencil tempGraphicStencil;
        if (config.graphicStencils.TryGetValue(item.shape, out tempGraphicStencil))
          graphicBox.Shape = tempGraphicStencil.ShapeTemplate();
        graphicBox.ZIndex = 0;
        graphicBox.AttachTo(modelBox, -50, -50, 150, 150);
        graphicBox.EnabledHandles = Handles.None;
        graphicBox.HandlesStyle = HandlesStyle.Invisible;
        graphicBox.Visible = false;

        graphicBox.ZTop();
        modelBox.ZTop();

        itemBoxes.Add(item.tag, new ItemBox(modelBox, graphicBox));
      }

      foreach (Link link in graphic.links.Values)
      {
        Box boxOrigin = itemBoxes[link.src].modelBox;
        Box boxDestination = itemBoxes[link.dst].modelBox;

        Arrow arrowOrigin = fcFlowChart.CreateArrow(boxOrigin, boxDestination);
        arrowOrigin.ArrowHead = ArrowHead.Triangle;
        arrowOrigin.Style = ArrowStyle.Cascading;
        arrowOrigin.Visible = false;
      }

      fcFlowChart.UndoManager.UndoEnabled = true;
      fcFlowChart.UseWaitCursor = false;
      fcFlowChart.ValidityChecks = true;
      fcFlowChart.Visible = true;
      fcFlowChart.Enabled = true;
      ResumeLayout(false);

      ZoomToVisible();
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
      itemBoxes[tag].graphicBox.Visible = visible;

      foreach (Arrow arrowDestination in itemBoxes[tag].modelBox.IncomingArrows)
      {
        arrowDestination.Visible = visible;
      }

      foreach (Arrow arrowOrigin in itemBoxes[tag].modelBox.OutgoingArrows)
      {
        arrowOrigin.Visible = visible;
      }
    }

    internal void SetSelected(string tag, bool selected)
    {
      itemBoxes[tag].modelBox.Selected = selected;
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
      PointCollection controlPoints = e.Arrow.ControlPoints.Clone();

      if (e.Arrow.Origin is DummyNode)
      {
        e.Arrow.Origin = savedOrigin;
        e.Arrow.OrgnAnchor = savedOriginAnchor;
      }

      if (e.Arrow.Destination is DummyNode)
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

    private void fcFlowChart_ArrowModifying(object sender, ArrowMouseArgs e)
    {
      savedOrigin = e.Arrow.Origin;
      savedDestination = e.Arrow.Destination;
      savedOriginAnchor = e.Arrow.OrgnAnchor;
      savedDestinationAnchor = e.Arrow.DestAnchor;
    }

    private Box visibleBox = null;
    private Arrow visibleArrow = null;

    private void fcFlowChart_MouseMove(object sender, MouseEventArgs e)
    {
      Box box = fcFlowChart.GetBoxAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)));
      if (box != null)
      {
        box = itemBoxes[box.Text].modelBox; // Deal with the modelbox, not the graphicbox.
        if ((visibleBox != null) && (visibleBox != box)) // we've just moved away from something, hide it.
        {
          visibleBox.Visible = false;
        }
        else if (visibleBox == null)
        {
          box.ZTop();
          box.Visible = true;
          visibleBox = box;
        }
      }
      else if (visibleBox != null)
      {
        visibleBox.Visible = false;
        visibleBox = null;
      }

      Arrow arrow = fcFlowChart.GetArrowAt(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y)), 1);
      if (arrow != null)
      {
        if ((visibleArrow != null) && (visibleArrow != arrow)) // we've just moved away from something, hide it.
        {
          visibleArrow.Destination.Visible = false;
          visibleArrow.Origin.Visible = false;
        }
        else if (visibleBox == null)
        {
          arrow.ZTop();
          arrow.Destination.ZTop();
          arrow.Origin.ZTop();
          arrow.Destination.Visible = true;
          arrow.Origin.Visible = true;
          visibleArrow = arrow;
        }
      }
      else if (visibleArrow != null)
      {
        visibleArrow.Destination.Visible = false;
        visibleArrow.Origin.Visible = false;
        visibleArrow = null;
      }
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
      Box graphicBox = itemBoxes[e.Box.Text].graphicBox;
      graphic.ModifyItem(graphicBox.Text, graphicBox.BoundingRect);
    }

    private void graphic_ItemModified(string tag, RectangleF boundingRect)
    {
      Box graphicBox = itemBoxes[tag].graphicBox;
      Box modelBox = itemBoxes[tag].modelBox;

      graphicBox.BoundingRect = boundingRect;
      modelBox.BoundingRect = new RectangleF(boundingRect.X + boundingRect.Width * 0.25F, 
        boundingRect.Y + boundingRect.Height * 0.25F,
        boundingRect.Width * 0.5F, boundingRect.Height * 0.5F);

      graphicBox.ZTop();
      modelBox.ZTop();

    }
  }
}