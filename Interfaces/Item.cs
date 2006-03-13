using System;
using System.Data;
using System.Data.OleDb;

namespace SysCAD.Interface
{
  [Serializable]
  public class Item
  {
    public String tag;
    public float x, y, width, height, angle;
    public String model;
    public String shape;

    public System.Drawing.Color fillColor;

    /// <summary>
    /// Set unique tag.
    /// </summary>
    /// <param name="tag">Unique tag name.</param>
    public Item(String tag)
    {
      this.tag = tag;
    }

    /// <summary>
    /// Extract all information from the database.
    /// </summary>
    /// <param name="connection">Database connection.</param>
    public void Populate(OleDbConnection connection)
    {
      OleDbDataReader itemReader = (new OleDbCommand("SELECT ClassID, InsertX, InsertY, ScaleX, ScaleY, Rotation FROM GraphicsUnits WHERE Tag='"+tag+"'", connection)).ExecuteReader();
      if(itemReader.Read()) 
      {
        //shape = itemReader.GetString(0);
        // Quick and dirty ClassId <-> shape mapping...
        //switch (shape)
        //{
        //  case "Tie-1":
        //    shape = "Tie-1";
        //    break;
        //  case "Feed_XPG-1":
        //    shape = "Feed_XPG-1";
        //    break;
        //  case "Valve-1":
        //    shape = "Valve-1";
        //    break;
        //  case "GControl":
        //    shape = "GControl";
        //    break;
        //  case "PID_Cont-1":
        //    shape = "GControl";
        //    break;
        //  default:
        //    shape = "Valve_Default";
        //    break;
        //}

        model = itemReader.GetString(0);
        shape = itemReader.GetString(0);
        x = (float)itemReader.GetDouble(1);
        y = (float)itemReader.GetDouble(2);
        width = (float)itemReader.GetDouble(3)*5;
        height = (float)itemReader.GetDouble(4)*5;
        angle = (float)itemReader.GetDouble(5);
      }
      itemReader.Close();
    }
  }
}
