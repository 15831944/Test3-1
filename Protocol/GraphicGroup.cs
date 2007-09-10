
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
  public class GraphicGroup
  {
    private RectangleF boundingRect;

    private Guid guid;
    private String tag;
    private String path;

    [NonSerialized]
    public Object other = null;

    public GraphicGroup(Guid guid, String tag)
    {
      this.guid = guid;
      this.tag = tag;
    }

    public GraphicGroup(String tag)
    {
      this.guid = Guid.NewGuid();
      this.tag = tag;
    }

    public ARectangleF BoundingRect
    {
      get { return boundingRect; }
      set { boundingRect = value; }
    }

    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }

    public Single Height
    {
      get { return boundingRect.Height; }
      set { boundingRect.Height = value; }
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

    public Single Width
    {
      get { return boundingRect.Width; }
      set { boundingRect.Width = value; }
    }

    public Single X
    {
      get { return boundingRect.X; }
      set { boundingRect.X = value; }
    }

    public Single Y
    {
      get { return boundingRect.Y; }
      set { boundingRect.Y = value; }
    }
  }
}
