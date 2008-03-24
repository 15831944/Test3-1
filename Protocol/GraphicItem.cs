
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
    private Double opacity;

    public GraphicItem()
    {
      tagArea = new Rectangle(0.0, 0.0, 0.0, 0.0);
      tagAngle = 0.0;
      tagFont = new Font();
      tagVisible = false;
      opacity = 1.0;
    }

    public GraphicItem(Guid guid, String tag)
      : base(guid, tag)
    {
      tagArea = new Rectangle(0.0, 0.0, 0.0, 0.0);
      tagAngle = 0.0;
      tagFont = new Font();
      tagVisible = false;
      opacity = 1.0;
    }

    public GraphicItem(String tag)
      : base(tag)
    {
      tagArea = new Rectangle(0.0, 0.0, 0.0, 0.0);
      tagAngle = 0.0;
      tagFont = new Font();
      tagVisible = false;
      opacity = 1.0;
    }

    public GraphicItem(Guid guid, String tag, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible, Double opacity)
      : base(guid, tag)
    {
      TagArea = tagArea;
      TagAngle = tagAngle;
      TagFont = tagFont;
      TagVisible = tagVisible;
      Opacity = opacity;
    }

    public GraphicItem(String tag, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible, Double opacity)
      : base(tag)
    {
      TagArea = tagArea;
      TagAngle = tagAngle;
      TagFont = tagFont;
      TagVisible = tagVisible;
      Opacity = opacity;
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

    public Double Opacity
    {
      get
      {
        if (opacity > 1.0) opacity = 1.0;
        if (opacity < 0.0) opacity = 0.0;
        return opacity;
      }
      set
      {
        opacity = value;
        if (opacity > 1.0) opacity = 1.0;
        if (opacity < 0.0) opacity = 0.0;
      }
    }

  }
}
