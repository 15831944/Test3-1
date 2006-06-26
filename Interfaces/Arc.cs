using System;

namespace SysCAD.Interface
{
	/// <summary>
	/// Summary description for ArcTemplate.
	/// </summary>
	[Serializable]
  public class Arc : Element
	{
    public float x;
    public float y;
    public float w;
    public float h;
    public float a;
    public float s;

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
  
    public override string ToString()
    {
      return "(" + x.ToString("N2") + ", " + y.ToString("N2") + ", " + w.ToString("N2") + ", " + h.ToString("N2") + ", " + a.ToString("N2") + ", " + s.ToString("N2") + ")";
    }
  }
}
