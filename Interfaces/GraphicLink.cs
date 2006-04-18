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
    private String tag;

    private String classID;
    private String source;
    private String destination;
    private String sourcePort;
    private String destinationPort;

    public List<PointF> controlPoints;

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
    public String Source
    {
      get { return source; }
      set { source = value; }
    }

    [CategoryAttribute("Model"),
     DescriptionAttribute("Destination item of the link."),
     ReadOnlyAttribute(true)]
    public String Destination
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

    /// <summary>
    /// 
    /// </summary>
    /// <param name="tag"></param>
		public GraphicLink(String tag)
		{
      this.tag = tag;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="connection"></param>
    public void Populate(OleDbConnection connection)
    {
      OleDbDataReader linkReader = (new OleDbCommand("SELECT SrcTag, DstTag, ClassID FROM ModelLinks WHERE Tag='"+tag+"'", connection)).ExecuteReader();
      if(linkReader.Read()) 
      {
        source = linkReader.GetString(0);
        destination = linkReader.GetString(1);
        classID = linkReader.GetString(2);
      }
      linkReader.Close();

      if (controlPoints == null)
        controlPoints = new List<PointF>();
      controlPoints.Clear();
      OleDbDataReader linklineReader = (new OleDbCommand("SELECT VertexX, VertexY FROM GraphicsLinkLines WHERE Tag='" + tag + "'", connection)).ExecuteReader();
      while (linklineReader.Read())
      {
        //controlPoints.Add(new PointF((float)linklineReader.GetDouble(0), -(float)linklineReader.GetDouble(1)));
      }
      linklineReader.Close();
    }
  }
}
