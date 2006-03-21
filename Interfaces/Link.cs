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
	public class Link
	{
    private String tag;
    private String origin;
    private String destination;

    public List<PointF> controlPoints = new List<PointF>();

    [CategoryAttribute("Model"),
    DescriptionAttribute("Tag name of the link.")]
    public String Tag
    {
      get { return tag; }
      set { tag = value; }
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
    public String Origin
    {
      get { return origin; }
      set { origin = value; }
    }



    /// <summary>
    /// 
    /// </summary>
    /// <param name="tag"></param>
		public Link(String tag)
		{
      this.tag = tag;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="connection"></param>
    public void Populate(OleDbConnection connection)
    {
      OleDbDataReader linkReader = (new OleDbCommand("SELECT SrcTag, DstTag FROM ModelLinks WHERE Tag='"+tag+"'", connection)).ExecuteReader();
      if(linkReader.Read()) 
      {
        origin = linkReader.GetString(0);
        destination = linkReader.GetString(1);
      }
      linkReader.Close();
    }
  }
}
