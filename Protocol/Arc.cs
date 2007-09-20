
using System;
using System.Globalization;

namespace SysCAD.Protocol
{

  /// <summary>
  /// Summary description for ArcTemplate.
  /// </summary>
  [Serializable]
  public class Arc : Element
  {

    public Double a;
    public Double h;
    public Double s;
    public Double w;
    public Double x;
    public Double y;

    public Arc()
    {
    }

    public Arc(Double x, Double y, Double w, Double h, Double a, Double s)
    {
      this.x = x;
      this.y = y;
      this.w = w;
      this.h = h;
      this.a = a;
      this.s = s;
    }

    public override String ToString()
    {
      return "(" + x.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   y.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   w.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   h.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   a.ToString("N2", CultureInfo.InvariantCulture) + ", " +
                   s.ToString("N2", CultureInfo.InvariantCulture) + ")";
    }
  }
}
