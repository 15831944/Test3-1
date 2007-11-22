
using System;
using System.Collections.Generic;
using System.Text;
using System.Timers;
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

    private bool selected = false;
    private bool hovered = false;
    private bool visible = false;

    public Dictionary<int, String> anchorIntToTag = new Dictionary<int, String>();
    public Dictionary<String, int> anchorTagToInt = new Dictionary<String, int>();

    private int opacity;
    private Timer opacityTimer = new Timer();

    public EditorNode(State state, GraphicNode graphicNode)
    {
      this.state = state;
      this.graphicNode = graphicNode;

      opacityTimer.Interval = 50;
      opacityTimer.Elapsed += new ElapsedEventHandler(opacityTimer_Elapsed);
    }

    public void opacityTimer_Elapsed(object source, ElapsedEventArgs e)
    {
      if (visible && (ModelBox.Selected || state.ShowModels || hovered))
      {
        opacity+=50;
        if (opacity > 220)
        {
          opacityTimer.Stop();
          opacity = 220;
        }
      }
      else
      {
        opacity-=50;
        if (opacity < 1)
        {
          opacityTimer.Stop();
          opacity = 1;
        }
      }

      ModelBox.FillColor = Color.FromArgb(opacity, System.Drawing.Color.FromArgb(220, 222, 184, 136));
      ModelBox.FrameColor = Color.FromArgb(opacity, System.Drawing.Color.FromArgb(255, 111, 92, 68));
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

      modelBox.Visible = true;

      GraphicBox.Visible = visible && state.ShowGraphics;
      TextBox.Visible = visible && graphicNode.TagVisible && state.ShowTags;

      bool linkHovered = false;

      foreach (Arrow arrow in ModelBox.IncomingArrows)
      {
        EditorLink link = (arrow.Tag as EditorLink);
        if (link.Hovered) linkHovered = true;
        if (link != null)
          link.UpdateVisibility();
        else
          state.ClientProtocol.LogMessage(out requestId, "EditorLink missing for Arrow (Tag: " + arrow.Text + ")", SysCAD.Log.MessageType.Error);
      }

      foreach (Arrow arrow in ModelBox.OutgoingArrows)
      {
        EditorLink link = (arrow.Tag as EditorLink);
        if (link.Hovered) linkHovered = true;
        if (link != null)
          link.UpdateVisibility();
        else
          state.ClientProtocol.LogMessage(out requestId, "EditorLink missing for Arrow (Tag: " + arrow.Text + ")", SysCAD.Log.MessageType.Error);
      }

      if ((hovered) || (linkHovered))
        ModelBox.CustomDraw = CustomDraw.Additional;
      else
        ModelBox.CustomDraw = CustomDraw.None;

      opacityTimer.Start();
    }
  }
}
