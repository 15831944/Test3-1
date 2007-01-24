 using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
//using System.Drawing;
using System.Drawing.Design;
using System.Collections;
using System.Collections.Generic;
//using System.Drawing.Drawing2D;



namespace SysCAD.Protocol
{
  [Serializable]
  public class GraphicThing
  {
    private Guid guid;
    private String path;
    private String tag;

    private String xaml;

    private System.Drawing.RectangleF boundingRect;
    
    private float angle;
    private bool mirrorX;
    private bool mirrorY;


    public GraphicThing(String tag)
    {
      this.guid = Guid.NewGuid();
      this.tag = tag;
    }


    public String Xaml
    {
      get { return xaml; }
      set { xaml = value; }
    }

    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public String Path
    {
      get { return path; }
      set { path = value; }
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

    public GraphicThing(Guid guid, String tag)
    {
      this.guid = guid;
      this.tag = tag;
    }

    public void Populate(String filename, String page, String guid, System.Drawing.RectangleF rectangle, String xaml, float angle, bool mirrorX, bool mirrorY)
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

      this.xaml = xaml;

      this.angle = angle;
    }

    public void Clear()
    {
      xaml = String.Empty;
    }
  }
}
