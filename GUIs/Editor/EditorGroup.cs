using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{

  internal class EditorArea
  {

    private Box box;

    private GraphicArea graphicArea;

    private bool visible;

    private State state;

    public EditorArea(State state, GraphicArea graphicArea)
    {
      this.state = state;
      this.graphicArea = graphicArea;
    }

    public override string ToString()
    {
      return graphicArea.Tag;
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

    //public GraphicArea GraphicArea
    //{
    //  get { return graphicArea; }
    //}

    public Guid Guid
    {
      get { return graphicArea.Guid; }
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
      box.Visible = visible && state.ShowAreas;
      box.ZBottom();
    }
  }
}
