using System;
using System.Collections.Generic;
using System.Data;
using System.Data.OleDb;
using System.Drawing;

namespace SysCAD.Interface
{
  [Serializable]
  public class GraphicArea
  {
    public String tag;
    public Dictionary<string, GraphicArea> graphicAreas;
    public Dictionary<string, GraphicItem> graphicItems;

    public GraphicArea(String tag)
    {
      this.tag = tag;
      graphicAreas = new Dictionary<string, GraphicArea>();
      graphicItems = new Dictionary<string, GraphicItem>();
    }

    public void Populate(OleDbConnection connection, Dictionary<string, GraphicItem> graphicItems,
                         Dictionary<string, GraphicLink> graphicLinks, ref float dX, ref float dY)
    {
      OleDbDataReader itemReader = (new OleDbCommand("SELECT Tag FROM GraphicsUnits WHERE Page='"+tag+"'", connection)).ExecuteReader(CommandBehavior.SingleResult);
      while(itemReader.Read()) 
      {
        this.graphicItems.Add(itemReader.GetString(0), graphicItems[itemReader.GetString(0)]);
      }
      itemReader.Close();

      float minX = Single.MaxValue, minY = Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

      foreach (string key in this.graphicItems.Keys)
      {
        GraphicItem graphicItem = this.graphicItems[key] as GraphicItem;
        if (graphicItem.X < minX)
          minX = graphicItem.X;
        if (graphicItem.Y < minY)
          minY = graphicItem.Y;
        if (graphicItem.X + graphicItem.Width > maxX)
          maxX = graphicItem.X + graphicItem.Width;
        if (graphicItem.Y + graphicItem.Height > maxY)
          maxY = graphicItem.Y + graphicItem.Height;
      }

      float scaleX = 320.0F / (maxX - minX);
      float scaleY = 240.0F / (maxY - minY);

      float scale;
      if (scaleX > scaleY)
        scale = scaleX;
      else
        scale = scaleY;

      foreach (string key in this.graphicItems.Keys)
      {
        GraphicItem graphicItem = this.graphicItems[key] as GraphicItem;

        // move origin to top-left.
        graphicItem.X -= minX;
        graphicItem.Y -= minY;

        // scale to 0:100
        graphicItem.X *= scale;
        graphicItem.Y *= scale;
        graphicItem.Width *= scale;
        graphicItem.Height *= scale;

        graphicItem.X += dX;
        graphicItem.Y += dY;

        foreach (GraphicLink graphicLink in graphicLinks.Values)
        {
          if ((graphicLink.Source == key))// || (link.Destination == key))
          {
            for (int i = 0; i < graphicLink.controlPoints.Count; i++)
            {
              graphicLink.controlPoints[i] = new PointF(graphicLink.controlPoints[i].X - minX, graphicLink.controlPoints[i].Y - minY);
              graphicLink.controlPoints[i] = new PointF(graphicLink.controlPoints[i].X * scale, graphicLink.controlPoints[i].Y * scale);
              graphicLink.controlPoints[i] = new PointF(graphicLink.controlPoints[i].X + dX, graphicLink.controlPoints[i].Y + dY);
            }
          }
        }
      }
    }
  }
}
