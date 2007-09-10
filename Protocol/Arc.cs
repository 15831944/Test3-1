
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

    public Single a;
    public Single h;
    public Single s;
    public Single w;
    public Single x;
    public Single y;

    public Arc()
    {
    }

    public Arc(Single x, Single y, Single w, Single h, Single a, Single s)
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
