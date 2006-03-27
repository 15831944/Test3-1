using System;
using System.Collections.Generic;
using System.Data;
using System.Data.OleDb;
using System.Drawing;

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
    public void Populate(OleDbConnection connection, Dictionary<string, Item> items,
                         Dictionary<string, Link> links, ref float dX, ref float dY)
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
        if (item.X < minX)
          minX = item.X;
        if (item.Y < minY)
          minY = item.Y;
        if (item.X + item.Width > maxX)
          maxX = item.X + item.Width;
        if (item.Y + item.Height > maxY)
          maxY = item.Y + item.Height;
      }

      float scaleX = 320.0F / (maxX - minX);
      float scaleY = 240.0F / (maxY - minY);

      float scale;
      if (scaleX > scaleY)
        scale = scaleX;
      else
        scale = scaleY;

      foreach (string key in this.items.Keys)
      {
        Item item = this.items[key] as Item;

        // move origin to top-left.
        item.X -= minX;
        item.Y -= minY;

        // scale to 0:100
        item.X *= scale;
        item.Y *= scale;
        item.Width *= scale;
        item.Height *= scale;

        item.X += dX;
        item.Y += dY;

        foreach (Link link in links.Values)
        {
          if ((link.Origin == key))// || (link.Destination == key))
          {
            for (int i = 0; i < link.controlPoints.Count; i++)
            {
              link.controlPoints[i] = new PointF(link.controlPoints[i].X - minX, link.controlPoints[i].Y - minY);
              link.controlPoints[i] = new PointF(link.controlPoints[i].X * scale, link.controlPoints[i].Y * scale);
              link.controlPoints[i] = new PointF(link.controlPoints[i].X + dX, link.controlPoints[i].Y + dY);
            }
          }
        }
      }
    }
  }
}
