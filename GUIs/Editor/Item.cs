
using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{

  internal class Item
  {

    private Box graphic;

    private GraphicItem graphicItem;

    private Guid guid;
    private Box model;
    private PureComponents.TreeView.Node node;
    private bool selected;
    private String tag;
    private Box text;

    private bool visible;

    public Item(Guid guid, String tag, Box model, Box graphic, Box text, bool visible, GraphicItem graphicItem)
    {
      this.guid = guid;
      this.tag = tag;
      this.model = model;
      this.graphic = graphic;
      this.text = text;
      this.visible = visible;
      this.graphicItem = graphicItem;
    }

    public override string ToString()
    {
      return tag;
    }

    internal void Remove(FlowChart flowChart)
    {

      if (Model != null)
        flowChart.DeleteObject(Model);

      if (Graphic != null)
        flowChart.DeleteObject(Graphic);

      if (Text != null)
        flowChart.DeleteObject(Text);

      if (Node != null)
        Node.Remove();
    }

    public Box Graphic
    {
      get { return graphic; }
      set { graphic = value; }
    }

    public GraphicItem GraphicItem
    {
      get { return graphicItem; }
    }

    public Guid Guid
    {
      get { return guid; }
    }

    public ArrowCollection IncomingArrows
    {
      get { return model.IncomingArrows; }
    }

    public Box Model
    {
      get { return model; }
      set { model = value; }
    }

    public PureComponents.TreeView.Node Node
    {
      get { return node; }
      set { node = value; }
    }

    public ArrowCollection OutgoingArrows
    {
      get { return model.OutgoingArrows; }
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

    public Box Text
    {
      get { return text; }
      set { text = value; }
    }

    public bool Visible
    {
      get { return visible; }
      set { visible = value; }
    }
  }
}
