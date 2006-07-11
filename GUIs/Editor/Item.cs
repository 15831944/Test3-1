using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Interface;

namespace SysCAD.Editor
{
  public class Item
  {
    private Guid guid;
    private String tag;
    private Box model;
    private Box graphic;
    private Box text;
    private PureComponents.TreeView.Node node;

    private GraphicItem graphicItem;

    private bool visible;
    private bool selected;

    public Guid Guid
    {
      get { return guid; }
    }

    public String Tag
    {
      get { return tag; }
      set { tag = value; }
    }

    public Box Model
    {
      get { return model; }
      set { model = value; }
    }

    public Box Graphic
    {
      get { return graphic; }
      set { graphic = value; }
    }

    public Box Text
    {
      get { return text; }
      set { text = value; }
    }

    public PureComponents.TreeView.Node Node
    {
      get { return node; }
      set { node = value; }
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

    public ArrowCollection IncomingArrows
    {
      get { return model.IncomingArrows; }
    }

    public ArrowCollection OutgoingArrows
    {
      get { return model.OutgoingArrows; }
    }

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
  }
}