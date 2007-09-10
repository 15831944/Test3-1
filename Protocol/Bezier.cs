
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

    public Single x1;
    public Single x2;
    public Single x3;
    public Single x4;
    public Single y1;
    public Single y2;
    public Single y3;
    public Single y4;

    public Bezier()
    {
    }

    public Bezier(Single x1, Single y1, Single x2, Single y2, Single x3, Single y3, Single x4, Single y4)
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
