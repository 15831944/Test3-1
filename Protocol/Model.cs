using System;
using System.Collections.Generic;
using System.Text;

namespace SysCAD.Protocol
{
  [Serializable]
  public class Model
  {
    private Dictionary<Guid, ModelNode> nodes;
    private Dictionary<Guid, ModelLink> links;
    
    public Model()
    {
      nodes = new Dictionary<Guid, ModelNode>();
      links = new Dictionary<Guid, ModelLink>();
    }

    public Dictionary<Guid, ModelNode> Nodes
    {
      get { return nodes; }
      set { nodes = value; }
    }

    public Dictionary<Guid, ModelLink> Links
    {
      get { return links; }
      set { links = value; }
    }

    public void Create(ModelItem modelItem)
    {
      if (modelItem is ModelNode)
        nodes.Add(modelItem.Guid, modelItem as ModelNode);

      if (modelItem is ModelLink)
        links.Add(modelItem.Guid, modelItem as ModelLink);
    }

    public void Modify(ModelItem modelItem)
    {
      if (nodes.ContainsKey(modelItem.Guid))
        nodes[modelItem.Guid] = modelItem as ModelNode;

      if (links.ContainsKey(modelItem.Guid))
        links[modelItem.Guid] = modelItem as ModelLink;
    }

    public void Delete(Guid guid)
    {
      if (nodes.ContainsKey(guid))
        nodes.Remove(guid);

      if (links.ContainsKey(guid))
        links.Remove(guid);
    }
  }
}
