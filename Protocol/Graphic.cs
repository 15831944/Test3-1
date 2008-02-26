using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Graphic
  {
    private Dictionary<Guid, GraphicArea> areas;
    private Dictionary<Guid, GraphicNode> nodes;
    private Dictionary<Guid, GraphicLink> links;

    public Graphic()
    {
      areas = new Dictionary<Guid, GraphicArea>();
      nodes = new Dictionary<Guid, GraphicNode>();
      links = new Dictionary<Guid, GraphicLink>();
    }

    public Dictionary<Guid, GraphicArea> Areas
    {
      get { return areas; }
      set { areas = value; }
    }

    public Dictionary<Guid, GraphicNode> Nodes
    {
      get { return nodes; }
      set { nodes = value; }
    }

    public Dictionary<Guid, GraphicLink> Links
    {
      get { return links; }
      set { links = value; }
    }

    public void Create(GraphicItem graphicItem)
    {
      if (graphicItem is GraphicArea)
        areas.Add(graphicItem.Guid, graphicItem as GraphicArea);
      
      if (graphicItem is GraphicNode)
        nodes.Add(graphicItem.Guid, graphicItem as GraphicNode);

      if (graphicItem is GraphicLink)
        links.Add(graphicItem.Guid, graphicItem as GraphicLink);
    }

    public void Modify(GraphicItem graphicItem)
    {
      if (areas.ContainsKey(graphicItem.Guid))
        areas[graphicItem.Guid] = graphicItem as GraphicArea;

      if (nodes.ContainsKey(graphicItem.Guid))
        nodes[graphicItem.Guid] = graphicItem as GraphicNode;

      if (links.ContainsKey(graphicItem.Guid))
        links[graphicItem.Guid] = graphicItem as GraphicLink;
    }

    public void Delete(Guid guid)
    {
      if (areas.ContainsKey(guid))
        areas.Remove(guid);

      if (nodes.ContainsKey(guid))
        nodes.Remove(guid);

      if (links.ContainsKey(guid))
        links.Remove(guid);
    }

    public GraphicItem Get(Guid guid)
    {
      if (areas.ContainsKey(guid))
        return areas[guid];

      if (nodes.ContainsKey(guid))
        return nodes[guid];

      if (links.ContainsKey(guid))
        return links[guid];

      return null;
    }
  }
}
