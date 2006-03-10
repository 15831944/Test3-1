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

    private Graphic graphic;
    private Config config;

    public bool ShowModels = false;
    public bool ShowGraphics = true;
    public bool ShowTag = true;
    public bool ShowArrows = true;

    public bool SelectArrows = false;
    public bool SelectItems = true;

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
      //graphic.ItemModified += new Graphic.ItemModifiedHandler(graphic_ItemModified);

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
        modelBox.Visible = ShowModels;

        graphicBox = fcFlowChart.CreateBox(item.x, item.y, item.width, item.height);
        graphicBox.Text = item.tag;
        graphicBox.ToolTip = item.tag;
        graphicBox.Style = BoxStyle.Shape;
        GraphicStencil tempGraphicStencil;
        if (config.graphicStencils.TryGetValue(item.shape, out tempGraphicStencil))
          graphicBox.Shape = tempGraphicStencil.ShapeTemplate();
        graphicBox.AttachTo(modelBox, -50, -50, 150, 150);
        graphicBox.EnabledHandles = Handles.None;
        graphicBox.HandlesStyle = HandlesStyle.Invisible;
        graphicBox.Visible = ShowGraphics;

        graphicBox.ZBottom();
        modelBox.ZTop();

        itemBoxes.Add(item.tag, new ItemBox(modelBox, graphicBox, true));
      }

      foreach (Link link in graphic.links.Values)
      {
        Box boxOrigin = itemBoxes[link.src].ModelBox;
        Box boxDestination = itemBoxes[link.dst].ModelBox;

        Arrow arrow = fcFlowChart.CreateArrow(boxOrigin, boxDestination);
        arrow.ArrowHead = ArrowHead.Triangle;
        arrow.Style = ArrowStyle.Cascading;
        arrow.Visible = ShowArrows;
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
      itemBoxes[tag].Visible = visible;
      itemBoxes[tag].ModelBox.Visible = visible && (itemBoxes[tag].ModelBox.Selected || ShowModels);
      itemBoxes[tag].GraphicBox.Visible = visible && ShowGraphics;

      foreach (Arrow arrowDestination in itemBoxes[tag].ModelBox.IncomingArrows)
      {
        arrowDestination.Visible = visible && ShowArrows;
      }

      foreach (Arrow arrowOrigin in itemBoxes[tag].ModelBox.OutgoingArrows)
      {
        arrowOrigin.Visible = visible && ShowArrows;
      }
    }

    internal void SetSelected(string tag, bool selected)
    {
      itemBoxes[tag].ModelBox.Selected = selected;
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

    private void fcFlowChart_ArrowModifying(object sender, ArrowMouseArgs e)
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
            arrow.Visible = ShowArrows;
          }

          foreach (Arrow arrow in oldHoverItemBox.OutgoingArrows)
          {
            arrow.ZTop();
            arrow.Visible = ShowArrows;
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
        oldHoverArrow.Visible = ShowArrows;
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
      e.Box.Style = BoxStyle.Shape;
    }

    private void fcFlowChart_BoxCreating(object sender, BoxConfirmArgs e)
    {
      e.Box.Style = BoxStyle.Shape;
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