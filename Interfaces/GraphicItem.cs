using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Collections.Generic;

namespace SysCAD.Interface
{
  [Serializable]
  public class GraphicItem
  {
    private Guid guid;
    private String tag;
    private String path;

    private Model model;

    private Shape shape;
    private RectangleF boundingRect;
    private float angle;
    private bool mirrorX = false;
    private bool mirrorY = false;
    private System.Drawing.Color fillColor;

    public Dictionary<String, int> anchorTagToInt = new Dictionary<string,int>();
    public Dictionary<int, String> anchorIntToTag = new Dictionary<int,string>();


    [CategoryAttribute("Model"),
     DescriptionAttribute("Guid of the item."),
     ReadOnlyAttribute(true),
     DisplayName("Guid")]
    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
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

    [CategoryAttribute("Model"),
     DescriptionAttribute("Tag name of the item.")]
    public String Tag
    {
      get { return tag; }
      set { tag = value; }
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
     DescriptionAttribute("Shape name to be used for displaying the item."),
     Browsable(true),
     DisplayName("Shape")]
    public Shape Shape
    {
      get { return shape; }
      set { shape = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Model type of the item."),
     ReadOnlyAttribute(true),
     DisplayName("Model")]
    public Model Model
    {
      get { return model; }
      set { model = value; }
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

    public GraphicItem(Guid guid, String tag)
    {
      this.guid = guid;
      this.tag = tag;
    }

    public GraphicItem(String tag)
    {
      this.guid = Guid.NewGuid();
      this.tag = tag;
    }

    public void Populate(String filename, OleDbConnection connection)
    {
      OleDbDataReader itemReader = (new OleDbCommand("SELECT ClassID, InsertX, InsertY, ScaleX, ScaleY, Rotation, Page FROM GraphicsUnits WHERE Tag='" + tag + "'", connection)).ExecuteReader();
      if (itemReader.Read())
      {
        path = "/" + filename + "/" + itemReader.GetString(6) + "/";

        model = itemReader.GetString(0);
        shape = itemReader.GetString(0);

        OleDbDataReader itemGuidReader = (new OleDbCommand("SELECT EqpGUID FROM ModelUnits WHERE Tag='" + tag + "'", connection)).ExecuteReader();
        if (itemGuidReader.Read())
          guid = new Guid(itemGuidReader.GetString(0));

        float sx = 1.0F; float sy = 1.0F; float dx = 0.0F; float dy = 0.0F;
        if (shape.Contains("Feed")) { sx = 0.666666667F; sy = 0.201060241F; }
        if (shape.Contains("Tie")) { sx = 0.0653333333F; sy = 0.0653333333F; }
        if (shape.Contains("Control")) { sx = 0.2F; sy = 0.2F; }
        if (shape.Contains("PID")) { sx = 0.2F; sy = 0.2F; }
        if (shape.Contains("Actuator")) { sx = 0.2F; sy = 0.2F; }
        if (shape.Contains("Transmitter")) { sx = 0.2F; sy = 0.2F; }
        if (shape.Contains("Valve")) { sx = 0.2F; sy = 0.2F; }
        if (shape.Contains("Pump")) { sx = 0.2F; sy = 0.2F; }
        if (shape.Contains("Contct")) { sx = 0.8F; sy = 1.2F; }
        if (shape.Contains("Tank")) { sx = 0.6348186528F; sy = 0.777777778F; dy = -2.45F; }
        if (shape.Contains("Washer")) { sx = 1.2F; sy = 0.4F; }
        if (shape.Contains("FiltPrss")) { sx = 1.2F; sy = 0.4F; }

        boundingRect.Width = (float)itemReader.GetDouble(3) * 30.0F * sx;
        boundingRect.Height = (float)itemReader.GetDouble(4) * 30.0F * sy;

        if (boundingRect.Width < 0.0F)
        {
          mirrorX = true;
          boundingRect.Width = -boundingRect.Width;
        }

        if (boundingRect.Height < 0.0F)
        {
          mirrorY = true;
          boundingRect.Height = -boundingRect.Height;
        }

        boundingRect.X = (float)itemReader.GetDouble(1) - boundingRect.Width / 2.0F + dx;
        boundingRect.Y = -(float)itemReader.GetDouble(2) - boundingRect.Height / 2.0F + dy;
        angle = (float)itemReader.GetDouble(5);
      }
      itemReader.Close();
    }

    public void Populate(String filename, String Page, String EqpGUID, String ClassID, float InsertX, float InsertY, float ScaleX, float ScaleY, float Rotation)
    {
      path = "/" + filename + "/" + Page + "/";
      model = ClassID;
      shape = ClassID;
      guid = new Guid(EqpGUID);

      float sx = 1.0F; float sy = 1.0F; float dx = 0.0F; float dy = 0.0F;
      if (shape.Contains("Feed")) { sx = 0.666666667F; sy = 0.201060241F; }
      if (shape.Contains("Tie")) { sx = 0.0653333333F; sy = 0.0653333333F; }
      if (shape.Contains("Control")) { sx = 0.2F; sy = 0.2F; }
      if (shape.Contains("PID")) { sx = 0.2F; sy = 0.2F; }
      if (shape.Contains("Actuator")) { sx = 0.2F; sy = 0.2F; }
      if (shape.Contains("Transmitter")) { sx = 0.2F; sy = 0.2F; }
      if (shape.Contains("Valve")) { sx = 0.2F; sy = 0.2F; }
      if (shape.Contains("Pump")) { sx = 0.2F; sy = 0.2F; }
      if (shape.Contains("Contct")) { sx = 0.8F; sy = 1.2F; }
      if (shape.Contains("Tank")) { sx = 0.6348186528F; sy = 0.777777778F; dy = -2.45F; }
      if (shape.Contains("Washer")) { sx = 1.2F; sy = 0.4F; }
      if (shape.Contains("FiltPrss")) { sx = 1.2F; sy = 0.4F; }

      boundingRect.Width = ScaleX * 30.0F * sx;
      boundingRect.Height = ScaleY * 30.0F * sy;

      if (boundingRect.Width < 0.0F)
      {
        mirrorX = true;
        boundingRect.Width = -boundingRect.Width;
      }

      if (boundingRect.Height < 0.0F)
      {
        mirrorY = true;
        boundingRect.Height = -boundingRect.Height;
      }

      boundingRect.X = InsertX - boundingRect.Width / 2.0F + dx;
      boundingRect.Y = -InsertY - boundingRect.Height / 2.0F + dy;
      angle = Rotation;

    }

    public void Populate(String filename, String Page, String EqpGUID, String ClassID, RectangleF rectangle, float Rotation)
    {
      path = "/" + filename + "/" + Page + "/";
      model = ClassID;
      shape = ClassID;
      guid = new Guid(EqpGUID);

      boundingRect = rectangle;

      if (boundingRect.Width < 0.0F)
      {
        mirrorX = true;
        boundingRect.Width = -boundingRect.Width;
      }

      if (boundingRect.Height < 0.0F)
      {
        mirrorY = true;
        boundingRect.Height = -boundingRect.Height;
      }

      angle = Rotation;
    }
  }
}
