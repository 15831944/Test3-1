using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections;
using System.IO;
using SysCAD.Interface;


namespace SysCAD.ThingEditor.ThingEditorCtrl
{
  /// <summary>
  /// 
  /// </summary>
  public class Ellipse : Outline
  {
    private float left;
    private float top;
    private float right;
    private float bottom;

    private int PointIndex;

    private bool pointedEllipse;

    // Constructor
    public Ellipse(float x, float y)
      : base()
    {
      left = x;
      top = y;
      right = x + width;
      bottom = y + height;

      pointedEllipse = false;
    }

    public Ellipse(BinaryReader br)
      : base()
    {
      Load(br);
    }
    // end Constructor

    public float Left
    {
      get
      {
        return left;
      }
      set
      {
        left = value;
      }
    }

    public float Top
    {
      get
      {
        return top;
      }
      set
      {
        top = value;
      }
    }

    public float Right
    {
      get
      {
        return right;
      }
      set
      {
        right = value;
      }
    }

    public float Bottom
    {
      get
      {
        return bottom;
      }
      set
      {
        bottom = value;
      }
    }

    internal override bool checkOverPoint(PointF p)
    {
      // check  for center point
      if (Math.Abs(p.X - (Left + (width / 2) - PointsX)) < 5 && Math.Abs(p.Y - (Top + (height / 2) - PointsY)) < 5)
        return true;
      // check for 1 - point
      if (Math.Abs(p.X - Left) < 5 && Math.Abs(p.Y - Top) < 5)
        return true;
      // check for 2 - point
      if (Math.Abs(p.X - (Left + width)) < 5 && Math.Abs(p.Y - Top) < 5)
        return true;
      // check for 3 - point
      if (Math.Abs(p.X - (Left + width)) < 5 && Math.Abs(p.Y - (Top + height)) < 5)
        return true;
      // check for 4 - point
      if (Math.Abs(p.X - Left) < 5 && Math.Abs(p.Y - (Top + height)) < 5)
        return true;
      return false;
    }

    internal override void Draw(Graphics g, Pen pen)
    {
      if (pointedEllipse)
      {
        pen = new Pen(Color.Yellow, 1);
        g.DrawEllipse(pen, Left, Top, width, height);
        pen.Dispose();
      }
      else
        g.DrawEllipse(pen, Left, Top, width, height);
    }

    internal override bool hitTest(PointF p)
    {
      // check  for center point
      if (Math.Abs(p.X - (Left + (width / 2) - PointsX)) < 5 && Math.Abs(p.Y - (Top + (height / 2) - PointsY)) < 5)
      {
        PointIndex = 0;
        return true;
      }
      // check for 1 - point
      if (Math.Abs(p.X - Left) < 5 && Math.Abs(p.Y - Top) < 5)
      {
        PointIndex = 1;
        return true;
      }
      // check for 2 - point
      if (Math.Abs(p.X - (Left + width)) < 5 && Math.Abs(p.Y - Top) < 5)
      {
        PointIndex = 2;
        return true;
      }
      // check for 3 - point
      if (Math.Abs(p.X - (Left + width)) < 5 && Math.Abs(p.Y - (Top + height)) < 5)
      {
        PointIndex = 3;
        return true;
      }
      // check for 4 - point
      if (Math.Abs(p.X - Left) < 5 && Math.Abs(p.Y - (Top + height)) < 5)
      {
        PointIndex = 4;
        return true;
      }
      return false;
    }

    internal override void updatePoint(PointF p)
    {
      switch (PointIndex)
      {
        case 0:
          left = p.X - (width / 2);
          top = p.Y - (height / 2);
          right = left + width;
          bottom = top + height;
          break;
        case 1:
          left = p.X;
          top = p.Y;
          width = right - p.X;
          height = bottom - p.Y;
          break;
        case 2:
          width = p.X - left;
          height = bottom - p.Y;
          top = p.Y;
          right = left + width; ;
          break;
        case 3:
          width = p.X - left;
          height = p.Y - top;
          right = p.X;
          bottom = p.Y;
          break;
        case 4:
          left = p.X;
          width = right - p.X;
          height = p.Y - top;
          bottom = p.Y;
          break;
      }
    }

    internal bool isPointedEllipse(PointF p)
    {
      // check  for center point
      if (Math.Abs(p.X - (Left + (width / 2) - PointsX)) < 5 && Math.Abs(p.Y - (Top + (height / 2) - PointsY)) < 5)
      {
        pointedEllipse = true;
        return true;
      }
      // check for 1 - point
      if (Math.Abs(p.X - Left) < 5 && Math.Abs(p.Y - Top) < 5)
      {
        pointedEllipse = true;
        return true;
      }
      // check for 2 - point
      if (Math.Abs(p.X - (Left + width)) < 5 && Math.Abs(p.Y - Top) < 5)
      {
        pointedEllipse = true;
        return true;
      }
      // check for 3 - point
      if (Math.Abs(p.X - (Left + width)) < 5 && Math.Abs(p.Y - (Top + height)) < 5)
      {
        pointedEllipse = true;
        return true;
      }
      // check for 4 - point
      if (Math.Abs(p.X - Left) < 5 && Math.Abs(p.Y - (Top + height)) < 5)
      {
        pointedEllipse = true;
        return true;
      }
      pointedEllipse = false;
      return false;
    }

    internal override void addMenuItems(ContextMenu cntMenu, PointF p)
    {
      // TODO
    }

    internal override void drawPoints(Graphics g, Brush br)
    {
      // 1 - point
      g.FillEllipse(br, (Left - PointsX),
          (Top - PointsY),
          PointsWidth,
          PointsHeight);
      // 2 - point
      g.FillEllipse(br, (Left + width - PointsX),
          (Top - PointsY),
          PointsWidth,
          PointsHeight);
      // 3  - point
      g.FillEllipse(br,
          (Left + width - PointsX),
          (Top + height - PointsY),
          PointsWidth,
          PointsHeight);
      // 4 - point
      g.FillEllipse(br,
          (Left - PointsX),
          (Top + height - PointsY),
          PointsWidth,
          PointsHeight);
      // center point
      g.FillEllipse(br,
          (Left + (width / 2) - PointsX),
          (Top + (height / 2) - PointsY),
          PointsWidth,
          PointsHeight);
    }

    internal override string saveNameAsText()
    {
      float _left = getPercent(ThingEditor.MinX, ThingEditor.MaxX, left);
      float _top = getPercent(ThingEditor.MinY, ThingEditor.MaxY, top);
      float _right = getPercent(ThingEditor.MinX, ThingEditor.MaxX, right);
      float _bottom = getPercent(ThingEditor.MinY, ThingEditor.MaxY, bottom);

      return "            new ArcTemplate(" + _left.ToString() + ", " +
                                              _top.ToString() + ", " +
                                              _right.ToString() + ", " +
                                              _bottom.ToString() + ", " +
                        "0" + ", " +
                        "360" + ")";
    }

    internal override int getSegmentIndex(PointF p)
    {
      return distanceToPointF(p);
    }

    private int distanceToPointF(PointF p)
    {
      int Value = -1;
      int Value1 = -1;
      int Value2 = -1;

      float ellipseCenterX = Left + width / 2;
      float ellipseCenterY = Top + height / 2;
      PointF ellipseCenter = new PointF(ellipseCenterX, ellipseCenterY);
      RectangleF rect = new RectangleF(left, top, width, height);

      PointF pt1 = new PointF(0, 0);
      PointF pt2 = new PointF(0, 0);
      getEllipseIntr(rect, ellipseCenter, p, ref pt1, ref pt2);
      float x = p.X;
      float y = p.Y;

      float x1 = pt1.X;
      float y1 = pt1.Y;

      float x2 = pt2.X;
      float y2 = pt2.Y;

      Value1 = (int)Math.Sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1));
      Value2 = (int)Math.Sqrt((x - x2) * (x - x2) + (y - y2) * (y - y2));

      if (Value1 < Value2)
        return Value1;

      if (Value2 < Value1)
        return Value2;

      return Value;
    }

    private void getEllipseIntr(RectangleF rcBox, PointF pt1, PointF pt2, ref PointF res1, ref PointF res2)
    {

      RectangleF rc = RectangleF.FromLTRB(Math.Min(pt1.X, pt2.X),
                                        Math.Min(pt1.Y, pt2.Y),
                                        Math.Max(pt1.X, pt2.X),
                                        Math.Max(pt1.Y, pt2.Y));

      float x1 = pt1.X;
      float y1 = pt1.Y;
      float x2 = pt2.X;
      float y2 = pt2.Y;

      if (x1 != x2)
      {
        float cx = (rcBox.Left + rcBox.Right) / 2;
        float cy = (rcBox.Top + rcBox.Bottom) / 2;
        float ea = (rcBox.Right - rcBox.Left) / 2;
        float eb = (rcBox.Bottom - rcBox.Top) / 2;
        float a = (y1 - y2) / (x1 - x2);
        float b = (x1 * y2 - x2 * y1) / (x1 - x2);

        double A = eb * eb + a * a * ea * ea;
        double B = 2 * a * (b - cy) * ea * ea - 2 * cx * eb * eb;
        double C = eb * eb * cx * cx + ea * ea * (b - cy) * (b - cy) - ea * ea * eb * eb;

        float X1, X2, Y1, Y2;

        X1 = (float)((-B + Math.Sqrt(B * B - 4 * A * C)) / (2 * A));
        X2 = (float)((-B - Math.Sqrt(B * B - 4 * A * C)) / (2 * A));
        Y1 = a * X1 + b;
        Y2 = a * X2 + b;

        res1.X = X1; res1.Y = Y1;
        res2.X = X2; res2.Y = Y2;
      }
      else
      {
        float cx = (rcBox.Left + rcBox.Right) / 2;
        float cy = (rcBox.Top + rcBox.Bottom) / 2;
        float ea = (rcBox.Right - rcBox.Left) / 2;
        float eb = (rcBox.Bottom - rcBox.Top) / 2;

        float X = x1;
        float Y1 = cy - (float)Math.Sqrt((1 - (X - cx) * (X - cx) / (ea * ea)) * eb * eb);
        float Y2 = cy + (float)Math.Sqrt((1 - (X - cx) * (X - cx) / (ea * ea)) * eb * eb);

        res1.X = (float)X; res1.Y = (float)Y1;
        res2.X = (float)X; res2.Y = (float)Y2;
      }
    }

    internal override void Load(System.IO.BinaryReader br)
    {
      Left = br.ReadInt32();
      Top = br.ReadInt32();
      width = br.ReadInt32();
      height = br.ReadInt32();
    }

    internal override void Save(System.IO.BinaryWriter bw)
    {
      bw.Write(Left);
      bw.Write(Top);
      bw.Write(width);
      bw.Write(height);
    }

    internal override float getPercent(float Min, float Max, float Coord)
    {
      return ((Coord - Min) / (Max - Min)) * 100.0F;
    }

    internal override RectangleF getBoundingRect()
    {
      return new RectangleF(
          Math.Min(left, right),
          Math.Min(top, bottom),
          Math.Abs(left - right),
          Math.Abs(top - bottom)
                          );
    }

    internal override MindFusion.FlowChartX.ElementTemplate[] getElements(RectangleF rect)
    {
      float _left = getPercent(rect.Left, rect.Right, left);
      float _top = getPercent(rect.Top, rect.Bottom, top);
      float _right = getPercent(rect.Left, rect.Right, right);
      float _bottom = getPercent(rect.Top, rect.Bottom, bottom);

      return new MindFusion.FlowChartX.ElementTemplate[] 
                {
                    new MindFusion.FlowChartX.ArcTemplate(
                    Math.Min(_left, _right),
                    Math.Min(_top, _bottom),
                    Math.Abs(_right - _left), 
                    Math.Abs(_bottom - _top),
                    0,
                    360            
                    )
                };
    }

    internal override void getElements(RectangleF rect, ArrayList list)
    {
      float _left = getPercent(rect.Left, rect.Right, left);
      float _top = getPercent(rect.Top, rect.Bottom, top);
      float _right = getPercent(rect.Left, rect.Right, right);
      float _bottom = getPercent(rect.Top, rect.Bottom, bottom);

      list.Add(new Arc(Math.Min(_left, _right),
               Math.Min(_top, _bottom),
               Math.Abs(_right - _left), 
               Math.Abs(_bottom - _top),
               0,
               360));
    }
  }
}
