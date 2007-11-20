
using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;

namespace SysCAD.Editor
{

  internal class EditorNode
  {
    private GraphicNode graphicNode;

    private Box graphicBox;
    private Box modelBox;
    private Box textBox;

    private PureComponents.TreeView.Node node;

    private bool selected;
    private bool visible;

    public Dictionary<int, String> anchorIntToTag = new Dictionary<int, String>();
    public Dictionary<String, int> anchorTagToInt = new Dictionary<String, int>();

    public EditorNode(GraphicNode graphicItem)
    {
      this.graphicNode = graphicItem;
    }

    public override string ToString()
    {
      return Tag;
    }

    internal void Remove(FlowChart flowChart)
    {
      if (ModelBox != null)
        flowChart.DeleteObject(ModelBox);

      if (GraphicBox != null)
        flowChart.DeleteObject(GraphicBox);

      if (TextBox != null)
        flowChart.DeleteObject(TextBox);

      if (Node != null)
        Node.Remove();
    }

    public Box GraphicBox
    {
      get { return graphicBox; }
      set { graphicBox = value; }
    }

    public Guid Guid
    {
      get
      {
        if (graphicNode != null)
          return graphicNode.Guid;
        else
          return Guid.Empty;
      }
    }

    public ArrowCollection IncomingArrows
    {
      get { return modelBox.IncomingArrows; }
    }

    public Box ModelBox
    {
      get { return modelBox; }
      set { modelBox = value; }
    }

    public PureComponents.TreeView.Node Node
    {
      get { return node; }
      set { node = value; }
    }

    public ArrowCollection OutgoingArrows
    {
      get { return modelBox.OutgoingArrows; }
    }

    public bool Selected
    {
      get { return selected; }
      set { selected = value; }
    }

    public String Tag
    {
      get
      {
        if (graphicNode != null)
          return graphicNode.Tag;
        else
          return string.Empty;
      }
    }

    public bool MirrorX
    {
      get
      {
        if (graphicNode != null)
          return graphicNode.MirrorX;
        else
          return false;
      }
    }

    public Guid ModelGuid
    {
      get
      {
        return ModelGuid;
      }
    }

    public bool MirrorY
    {
      get
      {
        if (graphicNode != null)
          return graphicNode.MirrorY;
        else
          return false;
      }
    }

    public Box TextBox
    {
      get { return textBox; }
      set { textBox = value; }
    }

    public bool Visible
    {
      get { return visible; }
    }

    internal void SetVisible(bool visible, State state)
    {
      this.visible = visible;

      SetVisible(state);
    }

    internal void SetVisible(State state)
    {
      Int64 requestId;

      ModelBox.Visible = visible && (ModelBox.Selected || state.ShowModels);
      GraphicBox.Visible = visible && state.ShowGraphics;
      TextBox.Visible = visible && graphicNode.TagVisible && state.ShowTags;

      foreach (Arrow arrowDestination in ModelBox.IncomingArrows)
      {
        EditorLink linkDestination = (arrowDestination.Tag as EditorLink);
        if (linkDestination != null)
        {
          linkDestination.SetVisible(state);
        }
        else
        {
          state.ClientProtocol.LogMessage(out requestId, "EditorLink missing for Arrow (Tag: " + arrowDestination.Text + ")", SysCAD.Log.MessageType.Error);
        }
      }

      foreach (Arrow arrowOrigin in ModelBox.OutgoingArrows)
      {
        EditorLink linkOrigin = (arrowOrigin.Tag as EditorLink);
        if (linkOrigin != null)
        {
          linkOrigin.SetVisible(state);
        }
        else
        {
          state.ClientProtocol.LogMessage(out requestId, "EditorLink missing for Arrow (Tag: " + arrowOrigin.Text + ")", SysCAD.Log.MessageType.Error);
        }
      }
    }
  }
}
