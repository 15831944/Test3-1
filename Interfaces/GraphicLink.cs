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

    private String classID;
    private Guid source;
    private Guid destination;
    private String sourcePort;
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
      get { return classID; }
      set { classID = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Source item of the link."),
     ReadOnlyAttribute(true)]
    public Guid Source
    {
      get { return source; }
      set { source = value; }
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
    public String SourcePort
    {
      get { return sourcePort; }
      set { sourcePort = value; }
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

    public GraphicLink(Guid guid, String tag)
    {
      this.guid = guid;
      this.tag = tag;
    }

    public void Populate(OleDbConnection connection, Dictionary<Guid, GraphicItem> graphicItems)
    {
      OleDbDataReader linkReader = (new OleDbCommand("SELECT SrcTag, DstTag, ClassID FROM ModelLinks WHERE Tag='"+tag+"'", connection)).ExecuteReader();
      if(linkReader.Read()) 
      {
        OleDbDataReader sourceGuidReader = (new OleDbCommand("SELECT EqpGUID FROM ModelUnits WHERE Tag='" + linkReader.GetString(0) + "'", connection)).ExecuteReader();
        if (sourceGuidReader.Read())
          source = new Guid(sourceGuidReader.GetString(0));

        OleDbDataReader destinationGuidReader = (new OleDbCommand("SELECT EqpGUID FROM ModelUnits WHERE Tag='" + linkReader.GetString(1) + "'", connection)).ExecuteReader();
        if (destinationGuidReader.Read())
          destination = new Guid(destinationGuidReader.GetString(0));

        classID = linkReader.GetString(2);
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
      graphicItems.TryGetValue(source, out sourceItem);
      graphicItems.TryGetValue(destination, out destinationItem);

      float distanceSource0, distanceSourceN, distanceDestination0, distanceDestinationN;
      if ((sourceItem != null) && (controlPoints.Count > 0))
      {
        distanceSource0 = distance(controlPoints[0], sourceItem.BoundingRect);
        distanceSourceN = distance(controlPoints[controlPoints.Count - 1], sourceItem.BoundingRect);
      }
      else // make so that it won't ever reverse.
      {
        distanceSource0 = 0.0F;
        distanceSourceN = System.Single.MaxValue;
      }

      if ((destinationItem != null) && (controlPoints.Count > 0))
      {
        distanceDestination0 = distance(controlPoints[0], destinationItem.BoundingRect);
        distanceDestinationN = distance(controlPoints[controlPoints.Count - 1], destinationItem.BoundingRect);
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

    // Norm-1 distance between the closest side of the rectangle to the point.
    private float distance(PointF pointF, RectangleF rectangleF)
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
