
using System;
using System.Globalization;
using System.Drawing;

[assembly: CLSCompliant(true)]
namespace SysCAD.Protocol
{

  /// <summary>
  /// Summary description for LineElement.
  /// </summary>
  [Serializable]
  public class Line : Element
  {

    public Double x1;
    public Double x2;
    public Double y1;
    public Double y2;

    public Line()
    {
    }

    public Line(Double x1, Double y1, Double x2, Double y2)
    {
      this.x1 = x1;
      this.y1 = y1;
      this.x2 = x2;
      this.y2 = y2;
    }

    public override String ToString()
    {
      return "(" + x1.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   y1.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   x2.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   y2.ToString("N2", CultureInfo.InvariantCulture) + ")";
    }

    public PointF pointF1
    {
      get
      {
        return new PointF((float)x1, (float)y1);
      }

      set
      {
        x1 = (Double)value.X;
        y1 = (Double)value.Y;
      }
    }

    public PointF pointF2
    {
      get
      {
        return new PointF((float)x2, (float)y2);
      }

      set
      {
        x2 = (Double)value.X;
        y2 = (Double)value.Y;
      }
    }
  }
}
