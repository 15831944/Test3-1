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
    private Guid guid;
    private String tag;
    public Dictionary<string, GraphicArea> graphicAreas;
    public Dictionary<Guid, GraphicItem> graphicItems;

    public Guid Guid
    {
      get { return guid; }
      set { guid = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public GraphicArea(String tag)
    {
      this.guid = Guid.NewGuid();
      this.tag = tag;
      graphicAreas = new Dictionary<string, GraphicArea>();
      graphicItems = new Dictionary<Guid, GraphicItem>();
    }

    public GraphicArea(Guid guid, String tag)
    {
      this.guid = guid;
      this.tag = tag;
      graphicAreas = new Dictionary<string, GraphicArea>();
      graphicItems = new Dictionary<Guid, GraphicItem>();
    }

    public void Populate(OleDbConnection connection, Dictionary<Guid, GraphicItem> graphicItems,
                         Dictionary<Guid, GraphicLink> graphicLinks, ref float dX, ref float dY)
    {
      OleDbDataReader itemReader = (new OleDbCommand("SELECT Tag FROM GraphicsUnits WHERE Page='"+tag+"'", connection)).ExecuteReader(CommandBehavior.SingleResult);
      while(itemReader.Read()) 
      {
        Guid guid = new Guid();
        OleDbDataReader itemGuidReader = (new OleDbCommand("SELECT EqpGUID FROM ModelUnits WHERE Tag='" + itemReader.GetString(0) + "'", connection)).ExecuteReader();
        if (itemGuidReader.Read())
          guid = new Guid(itemGuidReader.GetString(0));

        GraphicItem graphicItem = graphicItems[guid];
        this.graphicItems.Add(graphicItem.Guid, graphicItem);
      }
      itemReader.Close();

      float minX = Single.MaxValue, minY = Single.MaxValue;
      float maxX = -Single.MaxValue, maxY = -Single.MaxValue;

      foreach (Guid guid in this.graphicItems.Keys)
      {
        GraphicItem graphicItem = this.graphicItems[guid] as GraphicItem;
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

      foreach (Guid guid in this.graphicItems.Keys)
      {
        GraphicItem graphicItem = this.graphicItems[guid] as GraphicItem;

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
          if ((graphicLink.Source == guid))// || (link.Destination == key))
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
