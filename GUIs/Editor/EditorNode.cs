
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
    public ModelNode modelNode;

    private Box graphicBox;
    private Box modelBox;
    private Box textBox;

    private PureComponents.TreeView.Node node;

    private bool selected = false;
    private bool hovered = false;
    private bool linkHovered = false;
    private bool visible = false;

    public Dictionary<int, String> anchorIntToTag = new Dictionary<int, String>();
    public Dictionary<String, int> anchorTagToInt = new Dictionary<String, int>();

    private int opacity;
    private Timer opacityTimer = new Timer();

    public EditorNode(State state, GraphicNode graphicNode, ModelNode modelNode)
    {
      this.state = state;
      this.graphicNode = graphicNode;
      this.modelNode = modelNode;

      opacityTimer.Interval = 100;
      opacityTimer.Elapsed += new ElapsedEventHandler(opacityTimer_Elapsed);
    }

    private delegate void SetOpacityDelegate(int opacity);

    internal void SetOpacity(int opacity)
      {
      if (ModelBox != null)
        {
        ModelBox.FillColor = Color.FromArgb(opacity, 222, 184, 136);
        ModelBox.FrameColor = Color.FromArgb(opacity, 111, 92, 68);

        foreach (AnchorPoint anchorPoint in ModelBox.AnchorPattern.Points)
          anchorPoint.Color = Color.FromArgb(opacity, anchorPoint.Color);
        }

        GraphicBox.FillColor = Color.FromArgb(220 - opacity, GraphicBox.FillColor);
        GraphicBox.FrameColor = Color.FromArgb(255 - opacity, GraphicBox.FrameColor);

      }

    public void opacityTimer_Elapsed(object source, ElapsedEventArgs e)
      {
      if (ModelBox == null)
        {
        opacityTimer.Stop();
        opacity = 1;
        }

      if (visible && (Selected || state.ShowModels || hovered || linkHovered))
        {
        opacity += 50;
        if (opacity > 170)
          {
          opacityTimer.Stop();
          opacity = 220;
          }
        }
      else
        {
        opacity -= 50;
        if (opacity < 51)
          {
          opacityTimer.Stop();
          opacity = 1;
          }
        }

      state.FlowChart.BeginInvoke(new SetOpacityDelegate(SetOpacity), new object[] { opacity });
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

        if (ModelBox != null)
          {
          foreach (Arrow arrow in ModelBox.IncomingArrows)
            (arrow.Tag as EditorLink).UpdateVisibility();
          foreach (Arrow arrow in ModelBox.OutgoingArrows)
            (arrow.Tag as EditorLink).UpdateVisibility();
          }

        UpdateVisibility();
      }
    }

    public bool Selected
    {
    get
      {
      if (ModelBox != null)
        {
        return ModelBox.Selected || GraphicBox.Selected || TextBox.Selected;
        }
      else if (GraphicBox != null)
        {
        return GraphicBox.Selected || TextBox.Selected;
        }
      else
        {
        return TextBox.Selected;
        }
      }
      set
      {
        selected = value;
        if (ModelBox != null)
          {
          ModelBox.Selected = value;
          GraphicBox.Selected = false;
          TextBox.Selected = false;
          }
        else if (GraphicBox != null)
          {
          GraphicBox.Selected = value;
          TextBox.Selected = false;
          }
        else
          {
          TextBox.Selected = value;
          }

        UpdateVisibility();
      }
    }

    public bool Hovered
    {
      get { return hovered; }
      set
      {
        hovered = value;

        foreach (Arrow arrow in ModelBox.IncomingArrows)
          (arrow.Tag as EditorLink).UpdateVisibility();
        foreach (Arrow arrow in ModelBox.OutgoingArrows)
          (arrow.Tag as EditorLink).UpdateVisibility();
        
        UpdateVisibility();
      }
    }

    internal void UpdateVisibility()
      {
      //Int64 requestId;

      if (ModelBox != null) ModelBox.Visible = visible;
      GraphicBox.Visible = visible && state.ShowGraphics;
      TextBox.Visible = visible && graphicNode.TagVisible && state.ShowTags;

      if (ModelBox != null) 
        {
        ModelBox.ZIndex = GraphicBox.ZIndex + 100;
        //linkHovered = false;
        foreach (Arrow arrow in ModelBox.IncomingArrows)
          {
          (arrow.Tag as EditorLink).UpdateVisibility();
          //arrow.ZIndex = Math.Max(arrow.Origin.ZIndex, arrow.Destination.ZIndex) + 10000;
          //if ((arrow.Tag as EditorLink).Hovered) 
          //  linkHovered = true;
          }
        foreach (Arrow arrow in ModelBox.OutgoingArrows)
          {
          (arrow.Tag as EditorLink).UpdateVisibility();
          // arrow.ZIndex = Math.Max(arrow.Origin.ZIndex, arrow.Destination.ZIndex) + 10000;
          //if ((arrow.Tag as EditorLink).Hovered) 
          //  linkHovered = true;
          }

        ModelBox.CustomDraw = CustomDraw.Additional;
        }

      TextBox.ZIndex = GraphicBox.ZIndex + 200;

      opacityTimer.Start();
      }
  }
}
