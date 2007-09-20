
using System;
using System.Drawing;
using System.Globalization;

namespace SysCAD.Protocol
{

  /// <summary>
  /// Summary description for BezierTemplate.
  /// </summary>
  [Serializable]
  public class Bezier : Element
  {

    public Double x1;
    public Double x2;
    public Double x3;
    public Double x4;
    public Double y1;
    public Double y2;
    public Double y3;
    public Double y4;

    public Bezier()
    {
    }

    public Bezier(Double x1, Double y1, Double x2, Double y2, Double x3, Double y3, Double x4, Double y4)
    {
      this.x1 = x1;
      this.y1 = y1;
      this.x2 = x2;
      this.y2 = y2;
      this.x3 = x3;
      this.y3 = y3;
      this.x4 = x4;
      this.y4 = y4;
    }

    //public PointF[] Points
    //{
    //  get
    //  {
    //    PointF[] points = new PointF[4];
    //    points[0] = new PointF(x1, y1);
    //    points[1] = new PointF(x2, y2);
    //    points[2] = new PointF(x3, y3);
    //    points[3] = new PointF(x4, y4);

    //    return points;
    //  }
    //}

    public override String ToString()
    {
      return "(" + x1.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   y1.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   x2.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   y2.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   x3.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   y3.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   x4.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   y4.ToString("N2", CultureInfo.InvariantCulture) + ")";
    }
  }
}
