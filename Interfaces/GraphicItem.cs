using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Collections.Generic;

namespace SysCAD.Interface
  {
  public class GlobalShapes
    {
    public static List<string> list = new List<string>();
    }

  public class ShapeConverter : StringConverter
    {
    public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
      {
      //true means show a combobox
      return true;
      }

    public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
      {
      //true will limit to list. false will show the list, 
      //but allow free-form entry
      return true;
      }

    public override System.ComponentModel.TypeConverter.StandardValuesCollection
           GetStandardValues(ITypeDescriptorContext context)
      {
      return new StandardValuesCollection(GlobalShapes.list.ToArray());
      }
    }

  [Serializable]
  public class GraphicItem
    {
    private Guid guid;
    private String tag;
    private String path;

    private String model;

    private String stencil;
    private float x;
    private float y;
    private float width;
    private float height;
    private float angle;
    private bool mirrorX = false;
    private bool mirrorY = false;
    public System.Drawing.Color fillColor;

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
    DisplayName("Area Path")]
    public String Path
      {
      get { return path; }
      }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Tag name of the item.")]
    public String Tag
      {
      get { return tag; }
      set { tag = value; }
      }

    //[CategoryAttribute("Graphic"), 
    // DescriptionAttribute("Bounding Rectangle if the Item")]
    //public RectangleF BoundingRect
    //{
    //  get
    //  {
    //    return new RectangleF(x, y, width, height);
    //  }
    //  set
    //  {
    //    x = value.X;
    //    y = value.Y;
    //    width = value.Width;
    //    height = value.Height;
    //  }
    //}

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Horizontal position of the center of the item.")]
    public float X
      {
      get { return x; }
      set { x = value; }
      }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Vertical position of the center of the item.")]
    public float Y
      {
      get { return y; }
      set { y = value; }
      }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Width of the item.")]
    public float Width
      {
      get { return width; }
      set { width = value; }
      }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Height of the item.")]
    public float Height
      {
      get { return height; }
      set { height = value; }
      }

    [CategoryAttribute("Graphic"),
    DescriptionAttribute("Bounding rectangle of the item."),
    ReadOnlyAttribute(true),
   DisplayName("Bounding Rect")]
    public RectangleF BoundingRect
      {
      get { return new RectangleF(x, y, width, height); }
      }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Angle of the item.")]
    public float Angle
      {
      get { return angle; }
      set { angle = value; }
      }

    [CategoryAttribute("Graphic"),
     DescriptionAttribute("Stencil name to be used for displaying the item."),
     Browsable(true),
     TypeConverter(typeof(ShapeConverter)),
     DisplayName("Stencil")]
    public string Shape
      {
      get { return stencil; }
      set { stencil = value; }
      }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Model type of the item."),
     ReadOnlyAttribute(true),
     DisplayName("Model")]
    public string Model
      {
      get { return model; }
      set { model = value; }
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
        stencil = itemReader.GetString(0);

        OleDbDataReader itemGuidReader = (new OleDbCommand("SELECT EqpGUID FROM ModelUnits WHERE Tag='" + tag + "'", connection)).ExecuteReader();
        if (itemGuidReader.Read())
          guid = new Guid(itemGuidReader.GetString(0));

        float sx = 1.0F; float sy = 1.0F; float dx = 0.0F; float dy = 0.0F;
        if (stencil.Contains("Feed")) { sx = 0.666666667F; sy = 0.201060241F; }
        if (stencil.Contains("Tie")) { sx = 0.0653333333F; sy = 0.0653333333F; }
        if (stencil.Contains("Control")) { sx = 0.2F; sy = 0.2F; }
        if (stencil.Contains("PID")) { sx = 0.2F; sy = 0.2F; }
        if (stencil.Contains("Actuator")) { sx = 0.2F; sy = 0.2F; }
        if (stencil.Contains("Transmitter")) { sx = 0.2F; sy = 0.2F; }
        if (stencil.Contains("Valve")) { sx = 0.2F; sy = 0.2F; }
        if (stencil.Contains("Pump")) { sx = 0.2F; sy = 0.2F; }
        if (stencil.Contains("Contct")) { sx = 0.8F; sy = 1.2F; }
        if (stencil.Contains("Tank")) { sx = 0.6348186528F; sy = 0.777777778F; dy = -2.45F; }
        if (stencil.Contains("Washer")) { sx = 1.2F; sy = 0.4F; }
        if (stencil.Contains("FiltPrss")) { sx = 1.2F; sy = 0.4F; }

        width = (float)itemReader.GetDouble(3) * 30.0F * sx;
        height = (float)itemReader.GetDouble(4) * 30.0F * sy;

        if (width < 0.0F)
          {
          mirrorX = true;
          width = -width;
          }

        if (height < 0.0F)
          {
          mirrorY = true;
          height = -height;
          }

        x = (float)itemReader.GetDouble(1) - width / 2.0F + dx;
        y = -(float)itemReader.GetDouble(2) - height / 2.0F + dy;
        angle = (float)itemReader.GetDouble(5);
        }
      itemReader.Close();
      }

    public void Populate(String filename, String Page, String EqpGUID, String ClassID, float InsertX, float InsertY, float ScaleX, float ScaleY, float Rotation)
      {
      path = "/" + filename + "/" + Page + "/";
      model = ClassID;
      stencil = ClassID;
      guid = new Guid(EqpGUID);

      float sx = 1.0F; float sy = 1.0F; float dx = 0.0F; float dy = 0.0F;
      if (stencil.Contains("Feed")) { sx = 0.666666667F; sy = 0.201060241F; }
      if (stencil.Contains("Tie")) { sx = 0.0653333333F; sy = 0.0653333333F; }
      if (stencil.Contains("Control")) { sx = 0.2F; sy = 0.2F; }
      if (stencil.Contains("PID")) { sx = 0.2F; sy = 0.2F; }
      if (stencil.Contains("Actuator")) { sx = 0.2F; sy = 0.2F; }
      if (stencil.Contains("Transmitter")) { sx = 0.2F; sy = 0.2F; }
      if (stencil.Contains("Valve")) { sx = 0.2F; sy = 0.2F; }
      if (stencil.Contains("Pump")) { sx = 0.2F; sy = 0.2F; }
      if (stencil.Contains("Contct")) { sx = 0.8F; sy = 1.2F; }
      if (stencil.Contains("Tank")) { sx = 0.6348186528F; sy = 0.777777778F; dy = -2.45F; }
      if (stencil.Contains("Washer")) { sx = 1.2F; sy = 0.4F; }
      if (stencil.Contains("FiltPrss")) { sx = 1.2F; sy = 0.4F; }

      width = ScaleX * 30.0F * sx;
      height = ScaleY * 30.0F * sy;

      if (width < 0.0F)
        {
        mirrorX = true;
        width = -width;
        }

      if (height < 0.0F)
        {
        mirrorY = true;
        height = -height;
        }

      x = InsertX - width / 2.0F + dx;
      y = -InsertY - height / 2.0F + dy;
      angle = Rotation;

      }
    }
  }
