 using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Collections;
using System.Collections.Generic;
using System.Drawing.Drawing2D;

namespace SysCAD.Interface
{
  [Serializable]
  public class GraphicThing
  {
    private Guid guid;
    private String path;
    private String tag;

    private ArrayList elements;
    private ArrayList decorations;
    private ArrayList textArea;
    private ArrayList animations;

    private FillMode fillMode;

    private String text;
    private RectangleF boundingRect;
    
    private float angle;
    private bool mirrorX;
    private bool mirrorY;
    private System.Drawing.Color fillColor;
    private System.Drawing.Color frameColor;

    public ArrayList Elements
    {
      get { return elements; }
      set { elements = value; }
    }

    public ArrayList Decorations
    {
      get { return decorations; }
      set { decorations = value; }
    }

    public ArrayList TextArea
    {
      get { return textArea; }
      set { textArea = value; }
    }

    public ArrayList Animations
    {
      get { return animations; }
      set { animations = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Guid of the item."),
     ReadOnlyAttribute(true),
     DisplayName("Guid")]
    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Tag name of the item.")]
    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Text of the item.")]
    public String Text
    {
      get { return text; }
      set { text = value; }
    }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Area path of the item."),
     ReadOnlyAttribute(true),
    DisplayName("Path")]
    public String Path
    {
      get { return path; }
      set { path = value; }
    }

    [CategoryAttribute("Graphic"),
    Browsable(false),
    DescriptionAttribute("Horizontal position of the center of the item.")]
    public float X
    {
      get { return boundingRect.X; }
      set { boundingRect.X = value; }
    }

    [CategoryAttribute("Graphic"),
   Browsable(false),
    DescriptionAttribute("Vertical position of the center of the item.")]
    public float Y
    {
      get { return boundingRect.Y; }
      set { boundingRect.Y = value; }
    }

    [CategoryAttribute("Graphic"),
   Browsable(false),
    DescriptionAttribute("Width of the item.")]
    public float Width
    {
      get { return boundingRect.Width; }
      set { boundingRect.Width = value; }
    }

    [CategoryAttribute("Graphic"),
   Browsable(false),
    DescriptionAttribute("Height of the item.")]
    public float Height
    {
      get { return boundingRect.Height; }
      set { boundingRect.Height = value; }
    }

    [CategoryAttribute("Graphic"),
    DescriptionAttribute("Bounding rectangle of the item."),
    //ReadOnlyAttribute(true),
   DisplayName("Bounding Rect")]
    public ARectangleF BoundingRect
    {
      get { return boundingRect; }
      set { boundingRect = value; }
    }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Angle of the item.")]
    public float Angle
    {
      get { return angle; }
      set { angle = value; }
    }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Color of fill."),
     DisplayName("Fill Color")]
    public System.Drawing.Color FillColor
    {
      get { return fillColor; }
      set { fillColor = value; }
    }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Mode of fill."),
     DisplayName("Fill Mode")]
    public System.Drawing.Drawing2D.FillMode FillMode
    {
      get { return fillMode; }
      set { fillMode = value; }
    }

    [CategoryAttribute("Graphic"),
    DescriptionAttribute("Color of frame."),
    DisplayName("Frame Color")]
    public System.Drawing.Color FrameColor
    {
      get { return frameColor; }
      set { frameColor = value; }
    }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Whether the item is flipped horizontally."),
     DisplayName("Mirror X")]
    public bool MirrorX
    {
      get { return mirrorX; }
      set { mirrorX = value; }
    }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Whether the item is flipped horizontally."),
     DisplayName("Mirror Y")]
    public bool MirrorY
    {
      get { return mirrorY; }
      set { mirrorY = value; }
    }

    public GraphicThing(Guid guid, String tag)
    {
      this.guid = guid;
      this.tag = tag;
    }

    public GraphicThing(String tag)
    {
      this.guid = Guid.NewGuid();
      this.tag = tag;
    }

    public void Populate(String filename, String page, String guid, RectangleF rectangle, float rotation, bool mirrorX, bool mirrorY, 
      System.Drawing.Color fillColor, System.Drawing.Color frameColor, String text, ArrayList elements, ArrayList decorations, ArrayList textArea, ArrayList animations, FillMode fillMode)
    {
      path = "/" + filename + "/" + page + "/";
      this.guid = new Guid(guid);

      boundingRect = rectangle;

      this.mirrorX = mirrorX;
      this.mirrorY = mirrorY;

      if (boundingRect.Width < 0.0F)
      {
        this.mirrorX = !this.mirrorX;
        boundingRect.Width = -boundingRect.Width;
      }

      if (boundingRect.Height < 0.0F)
      {
        this.mirrorY = !this.mirrorY;
        boundingRect.Height = -boundingRect.Height;
      }

      angle = rotation;

      this.fillColor = fillColor;
      this.frameColor = frameColor;
      this.text = text;
      this.elements = elements;
      this.decorations = decorations;
      this.textArea = textArea;
      this.animations = animations;
      this.fillMode = fillMode;
    }

    public void Clear()
    {
      elements = new ArrayList();
      decorations = new ArrayList();
      textArea = new ArrayList();
      animations = new ArrayList();
    }
  }
}
