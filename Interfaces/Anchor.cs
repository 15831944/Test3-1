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

  [Serializable]
  public class Anchor
	{
    public string tag;
    public AnchorType type;
    public PointF position;

    [System.Xml.Serialization.XmlIgnoreAttribute()]
    public object direction;

    public uint min = 0;
    public uint max = 0;

    public Anchor()
    {
    }

    public Anchor(string tag, AnchorType type, PointF position, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.position = position;
      this.min = min;
      this.max = max;
    }

    public Anchor(string tag, AnchorType type, float X, float Y, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.position = new PointF(X, Y);
      this.min = min;
      this.max = max;
    }
  }
}
