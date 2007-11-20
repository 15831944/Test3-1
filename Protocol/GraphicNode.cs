
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
  public class GraphicNode : GraphicItem
  {
    private String path;

    private Guid modelGuid;
    private Shape shape;

    private Rectangle boundingRect;
    private Double angle;

    private Rectangle tagArea;
    private Double tagAngle;
    private Boolean tagVisible;

    private System.Drawing.Color fillColor;
    private FillMode fillMode;
    private bool mirrorX;
    private bool mirrorY;


    public GraphicNode(Guid guid, String tag, String path, Guid modelGuid, Shape shape, Rectangle boundingRect,
      Double angle, Rectangle tagArea, Double tagAngle, Boolean tagVisible, System.Drawing.Color fillColor,
      FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.Guid = guid;
      this.Tag = tag;

      this.path = path;

      this.modelGuid = modelGuid;
      this.shape = shape;

      this.boundingRect = boundingRect;
      this.angle = angle;

      this.tagArea = tagArea;
      this.tagAngle = tagAngle;
      this.tagVisible = tagVisible;

      this.fillColor = fillColor;
      this.fillMode = fillMode;
      this.mirrorX = mirrorX;
      this.mirrorY = mirrorY;
    }

    public Double Angle
    {
      get { return angle; }
      set { angle = value; }
    }

    public Rectangle BoundingRect
    {
      get { return boundingRect; }
      set { boundingRect = value; }
    }

    public Double TagAngle
    {
      get { return tagAngle; }
      set { tagAngle = value; }
    }

    public Rectangle TagArea
    {
      get { return tagArea; }
      set { tagArea = value; }
    }

    public Boolean TagVisible
    {
      get { return tagVisible; }
      set { tagVisible = value; }
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

    public Double Height
    {
      get { return boundingRect.Height; }
      set { boundingRect.Height = value; }
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

    public Guid ModelGuid
    {
      get { return modelGuid; }
      set { modelGuid = value; }
    }

    public String Path
    {
      get { return path; }
      set { path = value; }
    }

    public Shape Shape
    {
      get { return shape; }
      set { shape = value; }
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

    public GraphicNode Clone()
    {
      GraphicNode graphicNode = new GraphicNode(Guid, Tag, path, modelGuid, shape, boundingRect, angle, tagArea, tagAngle, tagVisible, fillColor, fillMode, mirrorX, mirrorY);

            return graphicNode;
    }
  }
}
