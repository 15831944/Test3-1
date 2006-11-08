using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Interface;

namespace SysCAD.Editor
{
  public class Thing
  {
    private Guid guid;
    private String tag;
    private Box box;
    private PureComponents.TreeView.Node node;

    private GraphicThing graphicThing;

    private bool visible;
    private bool selected;

    public GraphicThing GraphicThing
    {
      get { return graphicThing; }
      set { graphicThing = value; }
    }

    public Guid Guid
    {
      get { return guid; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public PureComponents.TreeView.Node Node
    {
      get { return node; }
      set { node = value; }
    }

    public Box Box
    {
      get { return box; }
      set { box = value; }
    }

    public bool Visible
    {
      get { return visible; }
      set { visible = value; }
    }

    public bool Selected
    {
      get { return selected; }
      set { selected = value; }
    }

    public Thing(Guid guid, String tag, Box box, bool visible, GraphicThing graphicThing)
    {
      this.guid = guid;
      this.box = box;
      this.visible = visible;
      this.graphicThing = graphicThing;
    }

    internal void Remove(FlowChart flowChart)
    {
      if (box != null)
        flowChart.DeleteObject(box);
      if (Node != null)
        Node.Remove();
    }
  }
}