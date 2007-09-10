
using System;
using System.Globalization;

[assembly: CLSCompliant(true)]
namespace SysCAD.Protocol
{

  /// <summary>
  /// Summary description for LineElement.
  /// </summary>
  [Serializable]
  public class Line : Element
  {

    public Single x1;
    public Single x2;
    public Single y1;
    public Single y2;

    public Line()
    {
    }

    public Line(Single x1, Single y1, Single x2, Single y2)
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
  }
}
