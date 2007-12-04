using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{

  internal class EditorGroup
  {

    private Box box;

    private GraphicGroup graphicGroup;

    private bool visible;

    private State state;

    public EditorGroup(State state, GraphicGroup graphicGroup)
    {
      this.state = state;
      this.graphicGroup = graphicGroup;
    }

    public override string ToString()
    {
      return graphicGroup.Tag;
    }

    //internal void Remove(FlowChart flowChart)
    //{

    //  if (Box != null)
    //    flowChart.DeleteObject(Box);

    //  if (Node != null)
    //    Node.Remove();
    //}

    public Box Box
    {
      //get { return box; }
      set { box = value; }
    }

    //public GraphicGroup GraphicGroup
    //{
    //  get { return graphicGroup; }
    //}

    public Guid Guid
    {
      get { return graphicGroup.Guid; }
    }

    //public PureComponents.TreeView.Node Node
    //{
    //  get { return node; }
    //  set { node = value; }
    //}

    //public bool Selected
    //{
    //  get { return selected; }
    //  set { selected = value; }
    //}

    //public String Tag
    //{
    //  get { return tag; }
    //  set { tag = value; }
    //}

    public bool Visible
    {
      get { return visible; }
      set 
      { 
        visible = value;
        UpdateVisibility();
      }
    }

    internal void UpdateVisibility()
    {
      box.FillColor = System.Drawing.Color.FromArgb(80, 222, 184, 136);
      box.FrameColor = System.Drawing.Color.FromArgb(160, 111, 92, 68);
      if (visible && state.ShowGroups)
        box.Visible = visible;
      box.ZBottom();
    }
  }
}
