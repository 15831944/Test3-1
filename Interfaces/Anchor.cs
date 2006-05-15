using System;
using System.Collections;
using System.Drawing;

namespace SysCAD.Interface
{
  public enum AnchorType
  {
    Electrical = 0,
    Control,
    Process
  }

  public enum AnchorDirection
  {
    In = 0,
    Out
  }

  [Serializable]
  public class Anchor
	{
    public string tag;
    public AnchorType type;
    public AnchorDirection direction;
    public PointF position;
    public uint min = 0;
    public uint max = 0;

    public Anchor()
    {
    }

    public Anchor(string tag, AnchorType type, AnchorDirection direction, PointF position, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.direction = direction;
      this.position = position;
      this.min = min;
      this.max = max;
    }

    public Anchor(string tag, AnchorType type, AnchorDirection direction, float X, float Y, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.direction = direction;
      this.position = new PointF(X, Y);
      this.min = min;
      this.max = max;
    }
  }
}
