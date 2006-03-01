using System;

namespace SysCAD.Interface
{
  /// <summary>
  /// Summary description for BezierTemplate.
  /// </summary>
  [Serializable]
  public class Bezier : Element
  {
    public float x1;
    public float y1;
    public float x2;
    public float y2;
    public float x3;
    public float y3;
    public float x4;
    public float y4;
      
    public Bezier()
    {
    }

    public Bezier(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
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
  }
}
