using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Collections.Generic;
using System.Drawing.Drawing2D;

namespace SysCAD.Protocol
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
    private bool mirrorX;
    private bool mirrorY;
    private System.Drawing.Color fillColor;
    private FillMode fillMode;

    public Dictionary<String, int> anchorTagToInt = new Dictionary<String,int>();
    public Dictionary<int, String> anchorIntToTag = new Dictionary<int,String>();


    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }

    public String Path
    {
      get { return path; }
      set { path = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public float X
    {
      get { return boundingRect.X; }
      set { boundingRect.X = value; }
    }

    public float Y
    {
      get { return boundingRect.Y; }
      set { boundingRect.Y = value; }
    }

    public float Width
    {
      get { return boundingRect.Width; }
      set { boundingRect.Width = value; }
    }

    public float Height
    {
      get { return boundingRect.Height; }
      set { boundingRect.Height = value; }
    }

    public ARectangleF BoundingRect
    {
      get { return boundingRect; }
      set { boundingRect = value; }
    }

    public float Angle
    {
      get { return angle; }
      set { angle = value; }
    }

    public Shape Shape
    {
      get { return shape; }
      set { shape = value; }
    }

    public Model Model
    {
      get { return model; }
      set { model = value; }
    }

    public System.Drawing.Color FillColor
    {
      get { return fillColor; }
      set { fillColor = value; }
    }

    public FillMode FillMode
    {
      get { return fillMode; }
      set { fillMode = value; }
    }

    public bool MirrorX
    {
      get { return mirrorX; }
      set { mirrorX = value; }
    }

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

        fillColor = Color.FromArgb(255, 220, 220, 255);

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

    public void Populate(String filename, String page, String guid, String classId, float insertX, float insertY, float scaleX, float scaleY, float rotation   )
    {
      path = "/" + filename + "/" + page + "/";
      model = classId;
      shape = classId;
      this.guid = new Guid(guid);

      fillColor = Color.FromArgb(255, 220, 220, 255);

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

      boundingRect.Width = scaleX * 30.0F * sx;
      boundingRect.Height = scaleY * 30.0F * sy;

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

      boundingRect.X = insertX - boundingRect.Width / 2.0F + dx;
      boundingRect.Y = -insertY - boundingRect.Height / 2.0F + dy;
      angle = rotation;

    }

    public void Populate(String filename, String page, String guid, String classId, RectangleF rectangle, float rotation)
    {
      path = "/" + filename + "/" + page + "/";
      model = classId;
      shape = classId;
      this.guid = new Guid(guid);

      fillColor = Color.FromArgb(255, 220, 220, 255);

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

      angle = rotation;
    }
  }
}
