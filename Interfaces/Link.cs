using System;
using System.Data;
using System.Data.OleDb;

namespace SysCAD.Interface
{
	/// <summary>
	/// Internal representation of a link in the model.
	/// </summary>
	[Serializable]
	public class Link
	{
    /// <summary>
    /// 
    /// </summary>
    public String tag;
    /// <summary>
    /// 
    /// </summary>
    public String src;
    /// <summary>
    /// 
    /// </summary>
    public String dst;

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
        src = linkReader.GetString(0);
        dst = linkReader.GetString(1);
      }
      linkReader.Close();
    }
  }
}
