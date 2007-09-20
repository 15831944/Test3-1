
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
    private ArrayList positions;
    private String tag;
    private AnchorType type;
    private Int16 look;

    public Anchor()
    {
    }

    public Anchor(String tag, AnchorType type, Int16 look, Double positionX, Double positionY)//, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.look = look;
      this.positions = new ArrayList();
      this.positions.Add(new Point(positionX, positionY));
      //this.min = min;
      //this.max = max;
    }

    private Anchor(String tag, AnchorType type, Int16 look, PointF position)//, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.look = look;
      this.positions = new ArrayList();
      this.positions.Add(new Point(position.X, position.Y));
      //this.min = min;
      //this.max = max;
    }

    private Anchor(String tag, AnchorType type, Int16 look, ArrayList positions)//, uint min, uint max)
    {
      this.tag = tag;
      this.type = type;
      this.look = look;
      this.positions = positions;
      //this.min = min;
      //this.max = max;
    }

    public ArrayList Positions
    {
      get { return positions; }
      set { positions = value; }
    }

    //public uint min = 0;
    //public uint max = 0;

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public Int16 Look
    {
      get { return look; }
      set { look = value; }
    }
  }
}
