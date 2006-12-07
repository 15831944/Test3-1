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
    private String tag;
    private AnchorType type;
    private PointF position;

    [System.Xml.Serialization.XmlIgnoreAttribute()]
    public object direction;

    //public uint min = 0;
    //public uint max = 0;

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }


    public PointF Position
    {
      get { return position; }
      set { position = value; }
    }


    public Anchor()
    {
    }

    public Anchor(string tag, AnchorType type, PointF position)//, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.position = position;
      //this.min = min;
      //this.max = max;
    }

    public Anchor(string tag, AnchorType type, float x, float y)//, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.position = new PointF(x, y);
      //this.min = min;
      //this.max = max;
    }
  }
}
