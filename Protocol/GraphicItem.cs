
using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{
  [Serializable]
  public abstract class GraphicItem : Item
  {
    private Rectangle tagArea;
    private Double tagAngle;
    private Font tagFont;
    private Boolean tagVisible;

    public GraphicItem()
    {
      tagArea = new Rectangle(0.0, 0.0, 0.0, 0.0);
      tagAngle = 0.0;
      tagFont = new Font();
      tagVisible = false;
    }

    public GraphicItem(Guid guid, String tag)
      : base(guid, tag)
    {
      tagArea = new Rectangle(0.0, 0.0, 0.0, 0.0);
      tagAngle = 0.0;
      tagFont = new Font();
      tagVisible = false;
    }

    public GraphicItem(String tag)
      : base(tag)
    {
      tagArea = new Rectangle(0.0, 0.0, 0.0, 0.0);
      tagAngle = 0.0;
      tagFont = new Font();
      tagVisible = false;
    }

    public GraphicItem(Guid guid, String tag, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible)
      : base(guid, tag)
    {
      TagArea = tagArea;
      TagAngle = tagAngle;
      TagFont = tagFont;
      TagVisible = tagVisible;
    }

    public GraphicItem(String tag, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible)
      : base(tag)
    {
      TagArea = tagArea;
      TagAngle = tagAngle;
      TagFont = tagFont;
      TagVisible = tagVisible;
    }

    public Rectangle TagArea
    {
      get { return tagArea; }
      set
      {
        if (value == null) throw new NullReferenceException("TagArea cannot be null.");
        tagArea = value;
      }
    }

    public Double TagAngle
    {
      get { return tagAngle; }
      set { tagAngle = value; }
    }

    public Font TagFont
    {
      get { return tagFont; }
      set
      {
        if (value == null) throw new NullReferenceException("TagFont cannot be null.");
        tagFont = value;
      }
    }

    public Boolean TagVisible
    {
      get { return tagVisible; }
      set { tagVisible = value; }
    }

    public Double TagAreaX
    {
      get { return tagArea.X; }
      set { TagArea.X = value; }
    }

    public Double TagAreaY
    {
      get { return TagArea.Y; }
      set { tagArea.Y = value; }
    }

    public Double TagAreaWidth
    {
      get { return tagArea.Width; }
      set { tagArea.Width = value; }
    }

    public Double TagAreaHeight
    {
      get { return tagArea.Height; }
      set { tagArea.Height = value; }
    }

  }
}
