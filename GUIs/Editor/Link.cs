using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{
  public class Link
  {
    private Guid guid;

    private String tag;

    private Arrow arrow;

    private GraphicLink graphicLink;

    private bool visible;

    public Guid Guid
    {
      get { return guid; }
    }

    public String Tag
    {
      get { return tag; }
    }

    public Arrow Arrow
    {
      get { return arrow; }
      set { arrow = value; }
    }

    public GraphicLink GraphicLink
    {
      get { return graphicLink; }
    }

    public bool Visible
    {
      get { return visible; }
      set { visible = value; }
    }

    public Link(Guid guid, String tag, GraphicLink graphicLink)
    {
      this.guid = guid;
      this.tag = tag;
      this.graphicLink = graphicLink;
    }

    internal void Remove(FlowChart flowChart)
    {
      if (Arrow != null)
        flowChart.DeleteObject(Arrow);
    }
  }
}