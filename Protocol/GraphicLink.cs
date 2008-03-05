
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

    public GraphicLink(Guid guid, Guid modelGuid, String tag, Guid origin, Int16 originPortID,
      Guid destination, Int16 destinationPortID,
      List<Point> controlPoints, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible)
      : base(guid, tag, tagArea, tagAngle, tagFont, tagVisible)
    {
      this.modelGuid = modelGuid;
      this.origin = origin;
      this.originPortID = originPortID;
      this.destination = destination;
      this.destinationPortID = destinationPortID;

      this.controlPoints = new List<Point>();

      foreach (Point controlPoint in controlPoints)
        this.controlPoints.Add(controlPoint);
    }

    public GraphicLink(Guid modelGuid, String tag, Guid origin, Int16 originPortID,
      Guid destination, Int16 destinationPortID,
      List<Point> controlPoints, Rectangle tagArea, Double tagAngle, Font tagFont, Boolean tagVisible)
      : base(tag, tagArea, tagAngle, tagFont, tagVisible)
    {
      this.modelGuid = modelGuid;
      this.origin = origin;
      this.originPortID = originPortID;
      this.destination = destination;
      this.destinationPortID = destinationPortID;

      this.controlPoints = new List<Point>();

      foreach (Point controlPoint in controlPoints)
        this.controlPoints.Add(controlPoint);
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

    public GraphicLink Clone()
    {
      return new GraphicLink(Guid, ModelGuid, Tag, Origin, OriginPortID, Destination, DestinationPortID, ControlPoints, TagArea, TagAngle, TagFont, TagVisible);
    }
  }
}
