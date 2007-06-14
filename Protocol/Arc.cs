
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

    public float a;
    public float h;
    public float s;
    public float w;
    public float x;
    public float y;

    public Arc()
    {
    }

    public Arc(float x, float y, float w, float h, float a, float s)
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
