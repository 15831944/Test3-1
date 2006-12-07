using System;
using System.Data;
using System.Data.OleDb;
using System.ComponentModel;
using System.Collections.Generic;
using System.Drawing;

namespace SysCAD.Interface
{
  /// <summary>
  /// Internal representation of a link in the model.
  /// </summary>
  [Serializable]
  public class GraphicLink
  {
    private Guid guid;
    private String tag;

    private String classId;
    private Guid origin;
    private Guid destination;
    private String originPort;
    private String destinationPort;

    public List<PointF> controlPoints;

    public Guid Guid
    {
      get { return guid; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Tag name of the link.")]
    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("ClassID of the link."),
     ReadOnlyAttribute(true)]
    public String ClassID
    {
      get { return classId; }
      set { classId = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Source item of the link."),
     ReadOnlyAttribute(true)]
    public Guid Origin
    {
      get { return origin; }
      set { origin = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Destination item of the link."),
     ReadOnlyAttribute(true)]
    public Guid Destination
    {
      get { return destination; }
      set { destination = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Source item of the link."),
     ReadOnlyAttribute(true)]
    public String OriginPort
    {
      get { return originPort; }
      set { originPort = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Destination item of the link."),
     ReadOnlyAttribute(true)]
    public String DestinationPort
    {
      get { return destinationPort; }
      set { destinationPort = value; }
    }

    public GraphicLink(String tag)
    {
      this.guid = Guid.NewGuid();
      this.tag = tag;
    }

    public GraphicLink(Guid guid, String tag, String classId, Guid srcGuid, String srcPort, Guid dstGuid, String dstPort)
    {
      this.guid = guid;
      this.tag = tag;
      this.classId = classId;
      this.origin = srcGuid;
      this.originPort = srcPort;
      this.destination = dstGuid;
      this.destinationPort = dstPort;
    }

    public void Populate(OleDbConnection connection, Dictionary<Guid, GraphicItem> graphicItems)
    {
      OleDbDataReader linkReader = (new OleDbCommand("SELECT SrcTag, DstTag, ClassID FROM ModelLinks WHERE Tag='" + tag + "'", connection)).ExecuteReader();
      if (linkReader.Read())
      {
        OleDbDataReader sourceGuidReader = (new OleDbCommand("SELECT EqpGUID FROM ModelUnits WHERE Tag='" + linkReader.GetString(0) + "'", connection)).ExecuteReader();
        if (sourceGuidReader.Read())
          origin = new Guid(sourceGuidReader.GetString(0));

        OleDbDataReader destinationGuidReader = (new OleDbCommand("SELECT EqpGUID FROM ModelUnits WHERE Tag='" + linkReader.GetString(1) + "'", connection)).ExecuteReader();
        if (destinationGuidReader.Read())
          destination = new Guid(destinationGuidReader.GetString(0));

        classId = linkReader.GetString(2);
      }
      linkReader.Close();

      if (controlPoints == null)
        controlPoints = new List<PointF>();
      controlPoints.Clear();
      OleDbDataReader linklineReader = (new OleDbCommand("SELECT VertexX, VertexY FROM GraphicsLinkLines WHERE Tag='" + tag + "'", connection)).ExecuteReader();
      while (linklineReader.Read())
      {
        controlPoints.Add(new PointF((float)linklineReader.GetDouble(0), -(float)linklineReader.GetDouble(1)));
      }

      // Reverse the list if the first is closer to the destination and vice versa...

      GraphicItem sourceItem, destinationItem;
      graphicItems.TryGetValue(origin, out sourceItem);
      graphicItems.TryGetValue(destination, out destinationItem);

      float distanceSource0, distanceSourceN, distanceDestination0, distanceDestinationN;
      if ((sourceItem != null) && (controlPoints.Count > 0))
      {
        distanceSource0 = distance(controlPoints[0], (RectangleF)sourceItem.BoundingRect);
        distanceSourceN = distance(controlPoints[controlPoints.Count - 1], (RectangleF)sourceItem.BoundingRect);
      }
      else // make so that it won't ever reverse.
      {
        distanceSource0 = 0.0F;
        distanceSourceN = System.Single.MaxValue;
      }

      if ((destinationItem != null) && (controlPoints.Count > 0))
      {
        distanceDestination0 = distance(controlPoints[0], (RectangleF)destinationItem.BoundingRect);
        distanceDestinationN = distance(controlPoints[controlPoints.Count - 1], (RectangleF)destinationItem.BoundingRect);
      }
      else // make so that it won't ever reverse.
      {
        distanceDestination0 = System.Single.MaxValue;
        distanceDestinationN = 0.0F;
      }

      if ((distanceSource0 > distanceSourceN) && (distanceDestinationN > distanceDestination0))
      {
        controlPoints.Reverse();
      }

      linklineReader.Close();
    }

    public void Populate(String guid, String classId,
      String source, String destination,
      String sourcePort, String destinationPort,
      List<PointF> controlPts,
      Dictionary<Guid, GraphicItem> graphicItems)
    {
      this.classId = classId;
      this.guid = new Guid(guid);
      origin = new Guid(source);
      this.destination = new Guid(destination);
      originPort = sourcePort;
      this.destinationPort = destinationPort;

      controlPoints = controlPts;

      // Reverse the list if the first is closer to the destination and vice versa...

      GraphicItem sourceItem, destinationItem;
      graphicItems.TryGetValue(origin, out sourceItem);
      graphicItems.TryGetValue(this.destination, out destinationItem);

      float distanceSource0, distanceSourceN, distanceDestination0, distanceDestinationN;
      if ((sourceItem != null) && (controlPoints.Count > 0))
      {
        distanceSource0 = distance(controlPoints[0], (RectangleF)sourceItem.BoundingRect);
        distanceSourceN = distance(controlPoints[controlPoints.Count - 1], (RectangleF)sourceItem.BoundingRect);
      }
      else // make so that it won't ever reverse.
      {
        distanceSource0 = 0.0F;
        distanceSourceN = System.Single.MaxValue;
      }

      if ((destinationItem != null) && (controlPoints.Count > 0))
      {
        distanceDestination0 = distance(controlPoints[0], (RectangleF)destinationItem.BoundingRect);
        distanceDestinationN = distance(controlPoints[controlPoints.Count - 1], (RectangleF)destinationItem.BoundingRect);
      }
      else // make so that it won't ever reverse.
      {
        distanceDestination0 = System.Single.MaxValue;
        distanceDestinationN = 0.0F;
      }

      if ((distanceSource0 > distanceSourceN) && (distanceDestinationN > distanceDestination0))
      {
        controlPoints.Reverse();
      }

      //linklineReader.Close();
    }

    // Norm-1 distance between the closest side of the rectangle to the point.
    static private float distance(PointF pointF, RectangleF rectangleF)
    {
      float dXL = System.Math.Abs(pointF.X - rectangleF.Left);
      float dXR = System.Math.Abs(pointF.X - rectangleF.Right);
      float dX;
      if (dXL < dXR)
        dX = dXL;
      else
        dX = dXR;

      float dYT = System.Math.Abs(pointF.Y - rectangleF.Top);
      float dYB = System.Math.Abs(pointF.Y - rectangleF.Bottom);
      float dY;
      if (dYT < dYB)
        dY = dYT;
      else
        dY = dYB;

      return dX + dY;
    }
  }
}
