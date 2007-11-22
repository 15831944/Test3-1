
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
    private State state;
    private GraphicNode graphicNode;

    private Box graphicBox;
    private Box modelBox;
    private Box textBox;

    private PureComponents.TreeView.Node node;

    private bool selected;
    private bool hovered;
    private bool visible;

    public Dictionary<int, String> anchorIntToTag = new Dictionary<int, String>();
    public Dictionary<String, int> anchorTagToInt = new Dictionary<String, int>();

    public EditorNode(State state, GraphicNode graphicNode)
    {
      this.state = state;
      this.graphicNode = graphicNode;
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
      set
      {
        visible = value;
        UpdateVisibility();
      }
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

    internal void UpdateVisibility()
    {
      Int64 requestId;

      //ModelBox.Visible = visible && (ModelBox.Selected || state.ShowModels);

      Color modelBoxColor = Color.FromArgb((visible && (ModelBox.Selected || state.ShowModels)) ? 128 : 255, ModelBox.FillColor);
      ModelBox.FillColor = modelBoxColor;

      GraphicBox.Visible = visible && state.ShowGraphics;
      TextBox.Visible = visible && graphicNode.TagVisible && state.ShowTags;

      foreach (Arrow arrowDestination in ModelBox.IncomingArrows)
      {
        EditorLink linkDestination = (arrowDestination.Tag as EditorLink);
        if (linkDestination != null)
        {
          linkDestination.UpdateVisibility();
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
          linkOrigin.UpdateVisibility();
        }
        else
        {
          state.ClientProtocol.LogMessage(out requestId, "EditorLink missing for Arrow (Tag: " + arrowOrigin.Text + ")", SysCAD.Log.MessageType.Error);
        }
      }

      if (hovered)
      {
        ModelBox.CustomDraw = CustomDraw.Additional;
        ModelBox.ZIndex = GraphicBox.ZIndex + 100000;
        TextBox.ZIndex = GraphicBox.ZIndex - 100000;
        TextBox.Visible = Visible && state.ShowTags;

        foreach (Arrow arrow in IncomingArrows)
        {
          arrow.Visible = Visible;
          arrow.CustomDraw = CustomDraw.Additional;
          arrow.ZIndex = TextBox.ZIndex - 100000;
        }

        foreach (Arrow arrow in OutgoingArrows)
        {
          arrow.Visible = Visible;
          arrow.CustomDraw = CustomDraw.Additional;
          arrow.ZIndex = TextBox.ZIndex - 100000;
        }
      }
      else
      {
        GraphicBox.Visible = Visible && state.ShowGraphics;
        ModelBox.Visible = Visible && (ModelBox.Selected || state.ShowModels);
        ModelBox.CustomDraw = CustomDraw.None;
        ModelBox.ZIndex = GraphicBox.ZIndex + 100000;
        TextBox.ZIndex = GraphicBox.ZIndex - 100000;
        TextBox.Visible = Visible && state.ShowTags;

        foreach (Arrow arrow in IncomingArrows)
        {
          arrow.Visible = Visible && state.ShowLinks;
          arrow.CustomDraw = CustomDraw.None;
          arrow.ZIndex = TextBox.ZIndex - 100000;
        }

        foreach (Arrow arrow in OutgoingArrows)
        {
          arrow.Visible = Visible && state.ShowLinks;
          arrow.CustomDraw = CustomDraw.None;
          arrow.ZIndex = TextBox.ZIndex - 100000;
        }
      }
    }
  }
}
