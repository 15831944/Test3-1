using System;
using System.Drawing;
using System.Collections;
using System.IO;
using System.Drawing.Drawing2D;
using SysCAD.Interface;

namespace SysCAD.ThingEditor
{
  /// <summary>
  /// 
  /// </summary>
  public class Contour : SegmentedOutline
  {
    private Color fillColor;
    private int Coef;
    // Constructors
    public Contour(Color color, int grdSize)
    {
      if (grdSize > 25)
        Coef = 7;
      else
        Coef = 14;
      shapeSegments = new ArrayList();
      //            Segment s1 = new LineSegment(new PointF(10, 10),   new PointF(210, 10));
      //            Segment s2 = new LineSegment(new PointF(210, 10),  new PointF(210, 210));
      //            Segment s3 = new LineSegment(new PointF(210, 210), new PointF(10, 210));
      //            Segment s4 = new LineSegment(new PointF(10, 210),  new PointF(10, 10));

      Segment s1 = new LineSegment(new PointF(grdSize, grdSize), new PointF(Coef * grdSize, grdSize));
      Segment s2 = new LineSegment(new PointF(Coef * grdSize, grdSize), new PointF(Coef * grdSize, Coef * grdSize));
      Segment s3 = new LineSegment(new PointF(Coef * grdSize, Coef * grdSize), new PointF(grdSize, Coef * grdSize));
      Segment s4 = new LineSegment(new PointF(grdSize, Coef * grdSize), new PointF(grdSize, grdSize));

      shapeSegments.Add(s1);
      shapeSegments.Add(s2);
      shapeSegments.Add(s3);
      shapeSegments.Add(s4);

      fillColor = color;
    }

    public Contour(Color color, ArrayList elements)
    {
      shapeSegments = new ArrayList();
      if (elements != null)
      {
        foreach (Element element in elements)
        {
          if (element is Line)
          {
            Line line = element as Line;
            shapeSegments.Add(new LineSegment(new PointF(line.x1 * 3.0F + 10.0F, line.y1 * 3.0F + 10.0F), new PointF(line.x2 * 3.0F + 10.0F, line.y2 * 3.0F + 10.0F)));
          }
          if (element is Bezier)
          {
            Bezier bezier = element as Bezier;
            BezierSegment bezierSegment = new BezierSegment();
            bezierSegment.points[0] = new PointF(bezier.x1 * 3.0F + 10.0F, bezier.y1 * 3.0F + 10.0F);
            bezierSegment.points[1] = new PointF(bezier.x2 * 3.0F + 10.0F, bezier.y2 * 3.0F + 10.0F);
            bezierSegment.points[2] = new PointF(bezier.x3 * 3.0F + 10.0F, bezier.y3 * 3.0F + 10.0F);
            bezierSegment.points[3] = new PointF(bezier.x4 * 3.0F + 10.0F, bezier.y4 * 3.0F + 10.0F);
            shapeSegments.Add(BelongSegment);
          }
        }
      }
    }

    public Contour(BinaryReader br)
    {
      shapeSegments = new ArrayList();
      Load(br);
    }

    public Contour(Color color, ArrayList arr, PointF[] pStart, PointF[] point1, PointF[] point2, PointF[] pEnd)
    {
      Segment s = null;
      shapeSegments = new ArrayList();
      if (arr != null)
      {
        for (int i = 0; i < arr.Count; ++i)
        {
          if (arr[i] is LineSegment)
            s = new LineSegment(pStart[i], pEnd[i]);

          if (arr[i] is BezierSegment)
          {
            s = new BezierSegment();
            (s as BezierSegment).points[0] = pStart[i];
            (s as BezierSegment).points[1] = point1[i];
            (s as BezierSegment).points[2] = point2[i];
            (s as BezierSegment).points[3] = pEnd[i];
          }
          shapeSegments.Add(s);
        }
      }
      fillColor = color;
    }

    // end Constructors

    public Color FillColor
    {
      get
      {
        return fillColor;
      }
      set
      {
        fillColor = value;
      }
    }

    internal override void Draw(Graphics g, Pen pen)
    {
      base.Draw(g, pen);
      PointF[] points = new PointF[shapeSegments.Count];
      GraphicsPath graphPath = new GraphicsPath();
      SolidBrush brush = new SolidBrush(fillColor);

      for (int i = 0; i < shapeSegments.Count; ++i)
      {
        Segment s = (Segment)shapeSegments[i];

        if (s is LineSegment)
          graphPath.AddLine((s as LineSegment).Start.X,
                            (s as LineSegment).Start.Y,
                            (s as LineSegment).End.X,
                            (s as LineSegment).End.Y);

        if (s is BezierSegment)
          graphPath.AddBezier(
                              (s as BezierSegment).points[0].X, (s as BezierSegment).points[0].Y,
                              (s as BezierSegment).points[1].X, (s as BezierSegment).points[1].Y,
                              (s as BezierSegment).points[2].X, (s as BezierSegment).points[2].Y,
                              (s as BezierSegment).points[3].X, (s as BezierSegment).points[3].Y
                             );
        points[i] = s.GetEndPoint();
      }
      g.FillPath(brush, graphPath);
      brush.Dispose();
      graphPath.Dispose();
    }
  }
}
