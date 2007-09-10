
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

    private List<PointF> controlPoints;
    private Guid destination;
    private String destinationPort;
    private Guid guid;
    private Guid origin;
    private String originPort;
    private String tag;

    private RectangleF textArea;
    private Single textAngle;

    [NonSerialized]
    public Object other = null;

    public GraphicLink(Guid guid, String tag, String classId, Guid source, String sourcePort, Guid destination,
      String destinationPort, List<PointF> controlPoints, RectangleF textArea, Single textAngle)
    {
      this.guid = guid;
      this.tag = tag;
      this.classId = classId;
      this.origin = source;
      this.originPort = sourcePort;
      this.destination = destination;
      this.destinationPort = destinationPort;

      this.controlPoints = new List<PointF>();

      foreach (PointF controlPoint in controlPoints)
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
    static private Single distance(PointF pointF, RectangleF rectangleF)
    {
      Single dXL = System.Math.Abs(pointF.X - rectangleF.Left);
      Single dXR = System.Math.Abs(pointF.X - rectangleF.Right);
      Single dX;

      if (dXL < dXR)
        dX = dXL;

      else
        dX = dXR;

      Single dYT = System.Math.Abs(pointF.Y - rectangleF.Top);
      Single dYB = System.Math.Abs(pointF.Y - rectangleF.Bottom);
      Single dY;

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

    public List<PointF> ControlPoints
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

    public RectangleF TextArea
    {
      get { return textArea; }
      set { textArea = value; }
    }

    public Single TextAngle
    {
      get { return textAngle; }
      set { textAngle = value; }
    }
  }
}
