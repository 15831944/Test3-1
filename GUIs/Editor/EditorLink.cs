using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{
  internal class EditorLink
  {
    private Arrow arrow;
    private Box textBox;

    private GraphicLink graphicLink;
    private ModelLink modelLink;

    private Guid guid;

    private String tag;

    private State state;

    private bool selected = false;
    private bool hovered = false;

    public EditorLink(State state, Guid guid, String tag, GraphicLink graphicLink, ModelLink modelLink)
    {
      this.state = state;
      this.guid = guid;
      this.tag = tag;
      this.graphicLink = graphicLink;
      this.modelLink = modelLink;
    }

    internal void Remove(FlowChart flowChart)
    {

      if (Arrow != null)
        flowChart.DeleteObject(Arrow);
    }

    public Arrow Arrow
    {
      get { return arrow; }
      set { arrow = value; }
    }

    public Box TextBox
    {
      get { return textBox; }
      set { textBox = value; }
    }

    public GraphicLink GraphicLink
    {
      get { return graphicLink; }
    }

    public ModelLink ModelLink
    {
      get { return modelLink; }
    }

    public Guid Guid
    {
      get { return guid; }
    }

    public String Tag
    {
      get { return tag; }
    }

    public bool Selected
    {
      get { return selected; }
      set 
      { 
        selected = value;
        UpdateVisibility();
      }
    }
    public bool Hovered
    {
      get { return hovered; }
      set
      { 
        hovered = value;
        UpdateVisibility();
      }
    }

    Int64 requestId;

    public bool Visible
    {
      get
      {
        EditorNode origin = arrow.Origin.Tag as EditorNode;
        EditorNode destination = arrow.Destination.Tag as EditorNode;

        bool visible = true;

        if (origin != null)
          visible = visible && origin.Visible;
        else
          state.ClientProtocol.LogMessage(out requestId, "EditorNode missing for Origin.", SysCAD.Log.MessageType.Error);

        if (destination != null)
          visible = visible && destination.Visible;
        else
          state.ClientProtocol.LogMessage(out requestId, "EditorNode missing for Destination.)", SysCAD.Log.MessageType.Error);

        return visible;
      }
    }

    internal void UpdateVisibility()
    {
      arrow.Visible = Visible && state.ShowLinks;
      textBox.Visible = arrow.Visible && graphicLink.TagVisible && state.ShowTags;
      textBox.ZIndex = arrow.ZIndex - 100000;

      if (hovered)
      {
        arrow.CustomDraw = CustomDraw.Additional;
        arrow.ZTop();

        if (arrow.Destination is Box)
        {
          EditorNode node = arrow.Destination.Tag as EditorNode;
          node.ModelBox.ZIndex = arrow.ZIndex - 100000;
          node.GraphicBox.ZIndex = arrow.ZIndex - 200000;
          node.TextBox.ZIndex = arrow.ZIndex - 300000;
        }

        if (arrow.Origin is Box)
        {
          EditorNode node = arrow.Origin.Tag as EditorNode;
          node.ModelBox.ZIndex = arrow.ZIndex - 100000;
          node.GraphicBox.ZIndex = arrow.ZIndex - 200000;
          node.TextBox.ZIndex = arrow.ZIndex - 300000;
        }
      }
      else
      {
        arrow.CustomDraw = CustomDraw.None;

        if (arrow.Destination is Box)
        {
          EditorNode node = arrow.Destination.Tag as EditorNode;
          node.ModelBox.ZIndex = arrow.ZIndex - 100000;
          node.GraphicBox.ZIndex = arrow.ZIndex - 200000;
          node.TextBox.ZIndex = arrow.ZIndex - 300000;
        }

        if (arrow.Origin is Box)
        {
          EditorNode node = arrow.Origin.Tag as EditorNode;
          node.ModelBox.ZIndex = arrow.ZIndex - 100000;
          node.GraphicBox.ZIndex = arrow.ZIndex - 200000;
          node.TextBox.ZIndex = arrow.ZIndex - 300000;
        }

        arrow.Visible = true;
      }
    }
  }
}
