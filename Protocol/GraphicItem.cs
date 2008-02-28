
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

    public Rectangle TagArea
    {
      get { return tagArea; }
      set { tagArea = value; }
    }

    public Double TagAngle
    {
      get { return tagAngle; }
      set { tagAngle = value; }
    }

    public Font TagFont
    {
      get { return tagFont; }
      set { tagFont = value; }
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
