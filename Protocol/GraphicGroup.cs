
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

    public GraphicArea(Guid guid, String tag)
    {
      this.Guid = guid;
      this.Tag = tag;
    }

    public GraphicArea(String tag)
    {
      this.Guid = Guid.NewGuid();
      this.Tag = tag;
    }

    public Rectangle BoundingRect
    {
      get { return boundingRect; }
      set { boundingRect = value; }
    }

    public Double Height
    {
      get { return boundingRect.Height; }
      set { boundingRect.Height = value; }
    }

    public String Path
    {
      get { return path; }
      set { path = value; }
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
  }
}
