
using System;
using System.Collections;
using System.Drawing;

namespace SysCAD.Protocol
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

    [System.Xml.Serialization.XmlIgnoreAttribute()]
    private PointF position;
    private String tag;
    private AnchorType type;

    public Anchor()
    {
    }

    public Anchor(String tag, AnchorType type, Single positionX, Single positionY)//, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.position = new PointF(positionX, positionY);
      //this.min = min;
      //this.max = max;
    }

    private Anchor(String tag, AnchorType type, PointF position)//, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.position = position;
      //this.min = min;
      //this.max = max;
    }

    public PointF Position
    {
      get { return position; }
      set { position = value; }
    }

    //public uint min = 0;
    //public uint max = 0;

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }
  }
}
