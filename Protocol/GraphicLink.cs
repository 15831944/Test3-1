
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
  public class GraphicLink
  {

    private String classId;

    private List<Point> controlPoints;
    private Guid destination;
    private String destinationPort;
    private Guid guid;
    private Guid origin;
    private String originPort;
    private String tag;

    private Rectangle textArea;
    private Double textAngle;

    [NonSerialized]
    public Object other = null;

    public GraphicLink(Guid guid, String tag, String classId, Guid source, String sourcePort, Guid destination,
      String destinationPort, List<Point> controlPoints, Rectangle textArea, Double textAngle)
    {
      this.guid = guid;
      this.tag = tag;
      this.classId = classId;
      this.origin = source;
      this.originPort = sourcePort;
      this.destination = destination;
      this.destinationPort = destinationPort;

      this.controlPoints = new List<Point>();

      foreach (Point controlPoint in controlPoints)
        this.controlPoints.Add(controlPoint);

      this.textArea = textArea;
      this.textAngle = textAngle;
    }

    public GraphicLink(String tag)
    {
      this.guid = Guid.NewGuid();
      this.tag = tag;
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

    public String ClassID
    {
      get { return classId; }
      set { classId = value; }
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

    public String DestinationPort
    {
      get { return destinationPort; }
      set { destinationPort = value; }
    }

    public Guid Guid
    {
      get { return guid; }
    }

    public Guid Origin
    {
      get { return origin; }
      set { origin = value; }
    }

    public String OriginPort
    {
      get { return originPort; }
      set { originPort = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public Rectangle TextArea
    {
      get { return textArea; }
      set { textArea = value; }
    }

    public Double TextAngle
    {
      get { return textAngle; }
      set { textAngle = value; }
    }
  }
}
