
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

    public EditorLink(Guid guid, String tag, GraphicLink graphicLink, ModelLink modelLink)
    {
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

    Int64 requestId;

    internal void SetVisible(State state)
    {
      EditorNode origin = arrow.Origin.Tag as EditorNode;
      EditorNode destination = arrow.Destination.Tag as EditorNode;

      bool visible = true;
      if (origin != null)
      {
        visible = visible && origin.Visible;
      }
      else
      {
        state.ClientProtocol.LogMessage(out requestId, "EditorNode missing for Origin.", SysCAD.Log.MessageType.Error);
      }

      if (destination != null)
      {
        visible = visible && destination.Visible;
      }
      else
      {
        state.ClientProtocol.LogMessage(out requestId, "EditorNode missing for Destination.)", SysCAD.Log.MessageType.Error);
      }

      arrow.Visible = visible && state.ShowLinks;
      textBox.Visible = arrow.Visible && graphicLink.TagVisible && state.ShowTags;
      textBox.ZIndex = arrow.ZIndex - 100000;
    }
  }
}
