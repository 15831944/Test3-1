using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using MindFusion.FlowChartX;

namespace StencilEditor
{
  public partial class Form1 : Form
  {
    LinkedList<Arrow> elementList = new LinkedList<Arrow>();
    LinkedList<Arrow> decorationList = new LinkedList<Arrow>();
    PointF tagPosition;

    SizeF defaultSize = new SizeF(10.0F, 10.0F);
    String groupName;

    Box example;

    public Form1()
    {
      InitializeComponent();

      elementList = new LinkedList<Arrow>();
      decorationList = new LinkedList<Arrow>();
      tagPosition = new PointF(0.0F, 0.0F);
      defaultSize = new SizeF(10.0F, 10.0F);
      groupName = "";

      flowChart1.DocExtents = new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F);

      Box canvas = flowChart1.CreateBox(1.5F, 1.5F, 100.0F, 100.0F);
      canvas.ZBottom();
      canvas.CustomDraw = CustomDraw.ShadowOnly;
      canvas.FillColor = Color.FromArgb(40, canvas.FillColor);
      canvas.Style = BoxStyle.Shape;
      canvas.Shape = ShapeTemplate.FromId("Rectangle");
      canvas.Locked = true;

      Box tl = flowChart1.CreateBox(2.5F, 2.5F, 0.000001F, 0.000001F);
      Box tr = flowChart1.CreateBox(102.5F, 2.5F, 0.000001F, 0.000001F);
      Box br = flowChart1.CreateBox(102.5F, 102.5F, 0.000001F, 0.000001F);
      Box bl = flowChart1.CreateBox(2.5F, 102.5F, 0.000001F, 0.000001F);

      elementList.AddLast(flowChart1.CreateArrow(tl, tr));
      elementList.AddLast(flowChart1.CreateArrow(tr, br));
      elementList.AddLast(flowChart1.CreateArrow(br, bl));
      elementList.AddLast(flowChart1.CreateArrow(bl, tl));

      example = flowChart1.CreateBox(1.5F, 1.5F, 100.0F, 100.0F);
      example.CustomDraw = CustomDraw.ShadowOnly;
      example.Style = BoxStyle.Shape;
      example.Locked = true;
      SetShape();

      flowChart1.ZoomToRect(new RectangleF(-25.0F, -25.0F, 150.0F, 150.0F));
      flowChart1.Show();
    }

    Node origin;
    Node destination;
    PointF mousePoint;

    private void flowChart1_ArrowModified(object sender, ArrowMouseArgs e)
    {
      if (elementList.Find(e.Arrow) != null)
      {
        if ((e.SelectionHandle == 0) || (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1))
        {
          mousePoint = new PointF((float)(Math.Round((e.X + 2.5) / 5.0) * 5.0 - 2.5), (float)(Math.Round((e.Y + 2.5) / 5.0) * 5.0 - 2.5));

          PointCollection points = e.Arrow.ControlPoints.Clone();

          if (e.SelectionHandle == 0)
          {
            points.SetAt(0, mousePoint);

            origin.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }
          else if (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1)
          {
            points.SetAt(points.Count - 1, mousePoint);

            destination.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }

          origin.BoundingRect = new RectangleF(e.Arrow.ControlPoints[0], new SizeF(0.000001F, 0.000001F));
          destination.BoundingRect = new RectangleF(e.Arrow.ControlPoints[e.Arrow.ControlPoints.Count - 1], new SizeF(0.000001F, 0.000001F));

          e.Arrow.Origin = origin;
          e.Arrow.Destination = destination;

          e.Arrow.ControlPoints.Clear();
          foreach (PointF point in points.getArray())
          {
            e.Arrow.ControlPoints.Add(point);
          }
          e.Arrow.UpdateFromPoints();
        }
      }
      SetShape();
    }

    private void flowChart1_ArrowModifying(object sender, ArrowMouseArgs e)
    {
      if (elementList.Find(e.Arrow)!=null)
      {
        if ((e.SelectionHandle == 0) || (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1))
        {
          mousePoint = new PointF((float)(Math.Round((e.X + 2.5) / 5.0) * 5.0 - 2.5), (float)(Math.Round((e.Y + 2.5) / 5.0) * 5.0 - 2.5));

          origin = e.Arrow.Origin;
          destination = e.Arrow.Destination;

          PointCollection points = e.Arrow.ControlPoints.Clone();

          if (e.SelectionHandle == 0)
          {
            points.SetAt(0, mousePoint);

            origin.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }
          else if (e.SelectionHandle == e.Arrow.ControlPoints.Count - 1)
          {
            points.SetAt(points.Count - 1, mousePoint);

            destination.BoundingRect = new RectangleF(mousePoint, new SizeF(0.000001F, 0.000001F));
          }

          e.Arrow.Origin = origin;
          e.Arrow.Destination = destination;

          e.Arrow.ControlPoints.Clear();
          foreach (PointF point in points.getArray())
          {
            e.Arrow.ControlPoints.Add(point);
          }
          e.Arrow.UpdateFromPoints();
        }
      }
      SetShape();
    }

    public void SetShape()
    {
      ElementTemplate[] elements = new ElementTemplate[elementList.Count];
      LinkedListNode<Arrow> element = elementList.First;
      for (int i = 0; i < elementList.Count; i++)
      {
        Arrow arrow = element.Value;
        if (arrow.Style == ArrowStyle.Polyline)
        {
          elements[i] = new LineTemplate(arrow.ControlPoints[0].X - 2.5F, arrow.ControlPoints[0].Y - 2.5F,
                                         arrow.ControlPoints[1].X - 2.5F, arrow.ControlPoints[1].Y - 2.5F);
        }
        if (arrow.Style == ArrowStyle.Bezier)
        {
          elements[i] = new BezierTemplate(arrow.ControlPoints[0].X - 2.5F, arrow.ControlPoints[0].Y - 2.5F,
                                           arrow.ControlPoints[1].X - 2.5F, arrow.ControlPoints[1].Y - 2.5F,
                                           arrow.ControlPoints[2].X - 2.5F, arrow.ControlPoints[2].Y - 2.5F,
                                           arrow.ControlPoints[3].X - 2.5F, arrow.ControlPoints[3].Y - 2.5F);
        }
        element = element.Next;
      }

      ElementTemplate[] decorations = new ElementTemplate[decorationList.Count];
      LinkedListNode<Arrow> decoration = decorationList.First;
      for (int i = 0; i < decorationList.Count; i++)
      {
        Arrow arrow = decoration.Value;
        if (arrow.Style == ArrowStyle.Polyline)
        {
          decorations[i] = new LineTemplate(arrow.ControlPoints[0].X - 2.5F, arrow.ControlPoints[0].Y - 2.5F,
                                            arrow.ControlPoints[1].X - 2.5F, arrow.ControlPoints[1].Y - 2.5F);
        }
        if (arrow.Style == ArrowStyle.Bezier)
        {
          decorations[i] = new BezierTemplate(arrow.ControlPoints[0].X - 2.5F, arrow.ControlPoints[0].Y - 2.5F,
                                              arrow.ControlPoints[1].X - 2.5F, arrow.ControlPoints[1].Y - 2.5F,
                                               arrow.ControlPoints[2].X - 2.5F, arrow.ControlPoints[2].Y - 2.5F,
                                               arrow.ControlPoints[3].X - 2.5F, arrow.ControlPoints[3].Y - 2.5F);
        }
        decoration = decoration.Next;
      }

      ShapeTemplate shape = new ShapeTemplate(elements, decorations, null, FillMode.Winding);

      example.Shape = shape;
    }

    Arrow contextArrow;

    public float Distance(PointF p1, PointF p2)
    {
      return (float)(Math.Sqrt(Math.Pow(p2.X - p1.X, 2) + Math.Pow(p2.Y - p1.Y, 2)));
    }

    PointF mouseClickPos;

    private void flowChart1_MouseClick(object sender, MouseEventArgs e)
    {
      mouseClickPos = flowChart1.ClientToDoc(e.Location);

      if (e.Button == MouseButtons.Right)
      {
        ContextMenu contextMenu = new ContextMenu();

        contextArrow = flowChart1.GetArrowAt(mouseClickPos, 1.0F);
        if (contextArrow is Arrow)
        {
          MenuItem fillAreaMenu = new MenuItem("FillArea...");
          fillAreaMenu.MenuItems.Add(new MenuItem("&Split", new EventHandler(SplitFillAreaLine)));
          fillAreaMenu.MenuItems.Add(new MenuItem("&Remove", new EventHandler(RemoveFillAreaLine)));
          if (contextArrow.Style == ArrowStyle.Bezier)
            fillAreaMenu.MenuItems.Add(new MenuItem("Convert to &Line", new EventHandler(ConvertToLine)));
          if (contextArrow.Style == ArrowStyle.Polyline)
            fillAreaMenu.MenuItems.Add(new MenuItem("Convert to &Bezier", new EventHandler(ConvertToBezier)));
          contextMenu.MenuItems.Add(fillAreaMenu);
        }
        MenuItem decorationMenu = new MenuItem("Decoration...");
        decorationMenu.MenuItems.Add(new MenuItem("Insert &Line", new EventHandler(InsertDecorationLine)));
        decorationMenu.MenuItems.Add(new MenuItem("Insert &Bezier", new EventHandler(InsertDecorationBezier)));
        contextMenu.MenuItems.Add(decorationMenu);
        contextMenu.Show(flowChart1, e.Location);
      }
    }

    public void SplitFillAreaLine(object sender, EventArgs e)
    {
      Box originBox = contextArrow.Origin as Box;
      Box destinationBox = contextArrow.Destination as Box;
      Box newBox = flowChart1.CreateBox(mouseClickPos.X,
                                        mouseClickPos.Y, 
                                        originBox.BoundingRect.Width, 
                                        originBox.BoundingRect.Height);

      contextArrow.Destination = newBox;

      flowChart1.ArrowStyle = contextArrow.Style;
      Arrow newArrow = flowChart1.CreateArrow(newBox, destinationBox);
      elementList.AddAfter(elementList.Find(contextArrow), newArrow);

      ReshapeArrow(contextArrow, originBox.BoundingRect, newBox.BoundingRect);
      ReshapeArrow(newArrow, newBox.BoundingRect, destinationBox.BoundingRect);

      SetShape();
    }

    private void ReshapeArrow(Arrow contextArrow, RectangleF originRect, RectangleF destinationRect)
    {
      float srX = originRect.X;
      float srY = originRect.Y;

      float erX = destinationRect.X;
      float erY = destinationRect.Y;

      float drX = erX - srX;
      float drY = erY - srY;

      PointCollection points = contextArrow.ControlPoints.Clone();

      float saX = points[0].X;
      float saY = points[0].Y;

      float eaX = points[points.Count - 1].X;
      float eaY = points[points.Count - 1].Y;

      float daX = eaX - saX;
      float daY = eaY - saY;
      
      contextArrow.ControlPoints.Clear();
      foreach (PointF point in points.getArray())
      {
        PointF tempPoint = point;
        tempPoint.X = ((point.X - srX) * drX / daX) + srX;
        tempPoint.Y = ((point.Y - srY) * drY / daY) + srY;
        contextArrow.ControlPoints.Add(tempPoint);
      }
      contextArrow.UpdateFromPoints();
    }

    public void RemoveFillAreaLine(object sender, EventArgs e)
    {
      LinkedListNode<Arrow> prevArrow = elementList.Find(contextArrow).Previous;
      LinkedListNode<Arrow> nextArrow = elementList.Find(contextArrow).Next;

      if (prevArrow == null)
        prevArrow = elementList.Last;
      if (nextArrow == null)
        nextArrow = elementList.First;

      nextArrow.Value.Origin = prevArrow.Value.Destination;

      flowChart1.DeleteObject(contextArrow);
      elementList.Remove(contextArrow);

      SetShape();
    }

    public void ConvertToLine(object sender, EventArgs e)
    {
      contextArrow.Style = ArrowStyle.Polyline;
      flowChart1.ActiveObject = contextArrow;
    }

    public void ConvertToBezier(object sender, EventArgs e)
    {
      contextArrow.Style = ArrowStyle.Bezier;
      flowChart1.ActiveObject = contextArrow;
    }

    public void InsertDecorationLine(object sender, EventArgs e)
    {
      flowChart1.ArrowStyle = ArrowStyle.Polyline;
      decorationList.AddLast(flowChart1.CreateArrow(new PointF(10.0F, 10.0F), new PointF(20.0F, 20.0F)));
      SetShape();
    }

    public void InsertDecorationBezier(object sender, EventArgs e)
    {
      flowChart1.ArrowStyle = ArrowStyle.Bezier;
      decorationList.AddLast(flowChart1.CreateArrow(new PointF(10.0F, 10.0F), new PointF(20.0F, 20.0F)));
      SetShape();
    }
  }
}