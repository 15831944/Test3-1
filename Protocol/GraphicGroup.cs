
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
  public class GraphicArea : GraphicItem
  {
    private Rectangle boundingRect;

    private String path;

    public GraphicArea(Guid guid, String tag, Rectangle boundingRect, String path)
      : base(guid, tag)
    {
      BoundingRect = boundingRect;
      Path = path;
    }

    public GraphicArea(String tag, Rectangle boundingRect, String path)
      : base(tag)
    {
      BoundingRect = boundingRect;
      Path = path;
    }

    public GraphicArea(Guid guid, String tag, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible, Rectangle boundingRect, String path)
      : base(guid, tag, tagArea, tagAngle, tagFont, tagVisible)
    {
      BoundingRect = boundingRect;
      Path = path;
    }

    public GraphicArea(String tag, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible, Rectangle boundingRect, String path)
      : base(tag, tagArea, tagAngle, tagFont, tagVisible)
    {
      BoundingRect = boundingRect;
      Path = path;
    }

    public Rectangle BoundingRect
    {
      get { return boundingRect; }
      set
      {
        if (value == null) throw new NullReferenceException("BoundingRect cannot be null.");
        boundingRect = value;
      }
    }

    public String Path
    {
      get { return path; }
      set
      {
        if (value == null) throw new NullReferenceException("Path cannot be null.");
        path = value;
      }
    }

    public Double Height
    {
      get { return boundingRect.Height; }
      set { boundingRect.Height = value; }
    }

    public Double Width
    {
      get { return boundingRect.Width; }
      set { boundingRect.Width = value; }
    }

    public Double X
    {
      get { return boundingRect.X; }
      set { boundingRect.X = value; }
    }

    public Double Y
    {
      get { return boundingRect.Y; }
      set { boundingRect.Y = value; }
    }

    public GraphicArea Clone()
    {
      return new GraphicArea(Guid, Tag, BoundingRect, Path);
    }
  }
}
