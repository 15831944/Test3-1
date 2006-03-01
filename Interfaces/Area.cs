using System;
using System.Collections.Generic;
using System.Data;
using System.Data.OleDb;

namespace SysCAD.Interface
{
  /// <summary>
  /// Summary description for Class1.
  /// </summary>
  [Serializable]
  public class Area
  {
    /// <summary>
    /// 
    /// </summary>
    public String tag;
    /// <summary>
    /// 
    /// </summary>
    public Dictionary<string, Area> areas;
    /// <summary>
    /// 
    /// </summary>
    public Dictionary<string, Item> items;

    /// <summary>
    /// 
    /// </summary>
    /// <param name="tag"></param>
    public Area(String tag)
    {
      areas = new Dictionary<string, Area>();
      items = new Dictionary<string, Item>();

      this.tag = tag;
    }

    /// <summary>
    /// 
    /// </summary>
    /// <param name="connection"></param>
    public void Populate(OleDbConnection connection, Dictionary<string, Item> items, ref float dX)
    {
      OleDbDataReader itemReader = (new OleDbCommand("SELECT Tag FROM GraphicsUnits WHERE Page='"+tag+"'", connection)).ExecuteReader(CommandBehavior.SingleResult);
      while(itemReader.Read()) 
      {
        this.items.Add(itemReader.GetString(0), items[itemReader.GetString(0)]);
      }
      itemReader.Close();

      float minX = Single.MaxValue, minY = Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

      foreach (string key in this.items.Keys)
      {
        Item item = this.items[key] as Item;
        if (item.x < minX)
          minX = item.x;
        if (item.y < minY)
          minY = item.y;
        if (item.x + item.width > maxX)
          maxX = item.x + item.width;
        if (item.y + item.height > maxY)
          maxY = item.y + item.height;
      }

      float scaleX = (maxX - minX) / 160.0F;
      float scaleY = (maxY - minY) / 120.0F;

      float scale;
      if (scaleX > scaleY)
        scale = scaleX;
      else
        scale = scaleY;

      foreach (string key in this.items.Keys)
      {
        Item item = this.items[key] as Item;

        // move origin to top-left.
        item.x -= minX;
        item.y -= minY;

        // scale to 0:100
        item.x /= scale;
        item.y /= scale;
        item.width /= scale;
        item.height /= scale;

        item.x += dX;
      }

      dX += 170.0F;
    }
  }
}
