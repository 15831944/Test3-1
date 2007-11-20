
using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Collections.Generic;
using System.Drawing;

namespace SysCAD.Protocol
{

  /// <summary>
  /// Internal representation of a link in the model.
  /// </summary>
  [Serializable]
  public class GraphicLink : GraphicItem
  {

    private Guid modelGuid;

    private List<Point> controlPoints;
    private Guid origin;
    private Guid destination;
    private Int16 originPortID;
    private Int16 destinationPortID;

    private Rectangle tagArea;
    private Double tagAngle;
    private Boolean tagVisible;

    public GraphicLink()
    {
    }

    public GraphicLink(Guid guid, Guid modelGuid, String tag, Guid origin, Int16 originPortID, 
      Guid destination, Int16 destinationPortID, 
      List<Point> controlPoints, Rectangle tagArea, Double tagAngle, Boolean tagVisible)
    {
      this.Guid = guid;
      this.Tag = tag;
      this.modelGuid = modelGuid;
      this.origin = origin;
      this.originPortID = originPortID;
      this.destination = destination;
      this.destinationPortID = destinationPortID;

      this.controlPoints = new List<Point>();

      foreach (Point controlPoint in controlPoints)
        this.controlPoints.Add(controlPoint);

      this.tagArea = tagArea;
      this.tagAngle = tagAngle;
    }

    // Norm-1 distance between the closest side of the rectangle to the point.
    static private Double distance(PointF pointF, Rectangle Rectangle)
    {
      Double dXL = System.Math.Abs(pointF.X - Rectangle.Left);
      Double dXR = System.Math.Abs(pointF.X - Rectangle.Right);
      Double dX;

      if (dXL < dXR)
        dX = dXL;

      else
        dX = dXR;

      Double dYT = System.Math.Abs(pointF.Y - Rectangle.Top);
      Double dYB = System.Math.Abs(pointF.Y - Rectangle.Bottom);
      Double dY;

      if (dYT < dYB)
        dY = dYT;

      else
        dY = dYB;

      return dX + dY;
    }

    public Guid ModelGuid
    {
      get { return modelGuid; }
      set { modelGuid = value; }
    }

    public List<Point> ControlPoints
    {
      get { return controlPoints; }
      set { controlPoints = value; }
    }

    public Guid Destination
    {
      get { return destination; }
      set { destination = value; }
    }

    public Int16 DestinationPortID
    {
      get { return destinationPortID; }
      set { destinationPortID = value; }
    }

    public Guid Origin
    {
      get { return origin; }
      set { origin = value; }
    }

    public Int16 OriginPortID
    {
      get { return originPortID; }
      set { originPortID = value; }
    }

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

    public Boolean TagVisible
    {
      get { return tagVisible; }
      set { tagVisible = value; }
    }

    public GraphicLink Clone()
    {
      GraphicLink graphicLink = new GraphicLink();

      graphicLink.Tag = Tag;
      graphicLink.Guid = Guid;

      graphicLink.modelGuid = modelGuid;
      graphicLink.origin = origin;
      graphicLink.destination = destination;
      graphicLink.originPortID = originPortID;
      graphicLink.destinationPortID = destinationPortID;

      graphicLink.controlPoints = new List<Point>();

      foreach (Point controlPoint in controlPoints)
        graphicLink.controlPoints.Add(controlPoint);

      graphicLink.tagArea = tagArea;
      graphicLink.tagAngle = tagAngle;
      
      return graphicLink;
    }
  }
}
