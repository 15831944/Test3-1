
using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{

  internal class Thing
  {
    private Box box;

    private GraphicThing graphicThing;

    private Guid guid;
    private PureComponents.TreeView.Node node;
    private bool selected;
    private String tag;

    private bool visible;

    public Thing(Guid guid, String tag, Box box, bool visible, GraphicThing graphicThing)
    {
      this.guid = guid;
      this.tag = tag;
      this.box = box;
      this.visible = visible;
      this.graphicThing = graphicThing;
    }

    public Box Box
    {
      get { return box; }
      set { box = value; }
    }

    public GraphicThing GraphicThing
    {
      get { return graphicThing; }
      set { graphicThing = value; }
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

    //internal void Remove(FlowChart flowChart)
    //{
    //  if (box != null)
    //    flowChart.DeleteObject(box);
    //  if (Node != null)
    //    Node.Remove();
    //}
  }
}
