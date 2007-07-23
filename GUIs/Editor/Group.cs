using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{

  internal class Group
  {

    private Box box;

    private GraphicGroup graphicGroup;

    private Guid guid;
    private PureComponents.TreeView.Node node;
    private bool selected;
    private String tag;

    private bool visible;

    public Group(Guid guid, String tag, Box box, bool visible, GraphicGroup graphicGroup)
    {
      this.guid = guid;
      this.tag = tag;
      this.box = box;
      this.visible = visible;
      this.graphicGroup = graphicGroup;
    }

    public override string ToString()
    {
      return tag;
    }

    internal void Remove(FlowChart flowChart)
    {

      if (Box != null)
        flowChart.DeleteObject(Box);

      if (Node != null)
        Node.Remove();
    }

    public Box Box
    {
      get { return box; }
      set { box = value; }
    }

    public GraphicGroup GraphicGroup
    {
      get { return graphicGroup; }
    }

    public Guid Guid
    {
      get { return guid; }
    }

    public PureComponents.TreeView.Node Node
    {
      get { return node; }
      set { node = value; }
    }

    public bool Selected
    {
      get { return selected; }
      set { selected = value; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public bool Visible
    {
      get { return visible; }
      set { visible = value; }
    }
  }
}
