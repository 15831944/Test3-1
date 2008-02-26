using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Graphic
  {
    private Dictionary<Guid, GraphicArea> groups;
    private Dictionary<Guid, GraphicNode> nodes;
    private Dictionary<Guid, GraphicLink> links;
    private Dictionary<Guid, GraphicThing> things;

    public Graphic()
    {
      groups = new Dictionary<Guid, GraphicArea>();
      nodes = new Dictionary<Guid, GraphicNode>();
      links = new Dictionary<Guid, GraphicLink>();
      things = new Dictionary<Guid, GraphicThing>();
    }

    public Dictionary<Guid, GraphicArea> Areas
    {
      get { return groups; }
      set { groups = value; }
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

    public Dictionary<Guid, GraphicThing> Things
    {
      get { return things; }
      set { things = value; }
    }

    public void Create(GraphicItem graphicItem)
    {
      if (graphicItem is GraphicArea)
        groups.Add(graphicItem.Guid, graphicItem as GraphicArea);
      
      if (graphicItem is GraphicNode)
        nodes.Add(graphicItem.Guid, graphicItem as GraphicNode);

      if (graphicItem is GraphicLink)
        links.Add(graphicItem.Guid, graphicItem as GraphicLink);
    }

    public void Modify(GraphicItem graphicItem)
    {
      if (groups.ContainsKey(graphicItem.Guid))
        groups[graphicItem.Guid] = graphicItem as GraphicArea;

      if (nodes.ContainsKey(graphicItem.Guid))
        nodes[graphicItem.Guid] = graphicItem as GraphicNode;

      if (links.ContainsKey(graphicItem.Guid))
        links[graphicItem.Guid] = graphicItem as GraphicLink;
    }

    public void Delete(Guid guid)
    {
      if (groups.ContainsKey(guid))
        groups.Remove(guid);

      if (nodes.ContainsKey(guid))
        nodes.Remove(guid);

      if (links.ContainsKey(guid))
        links.Remove(guid);
    }

    public GraphicItem Get(Guid guid)
    {
      if (groups.ContainsKey(guid))
        return groups[guid];

      if (nodes.ContainsKey(guid))
        return nodes[guid];

      if (links.ContainsKey(guid))
        return links[guid];

      return null;
    }
  }
}
