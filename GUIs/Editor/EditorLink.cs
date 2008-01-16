using System;
using System.Collections.Generic;
using System.Text;
using MindFusion.FlowChartX;
using System.Drawing;
using SysCAD.Protocol;
using System.Timers;

namespace SysCAD.Editor
{
  internal class EditorLink
  {
    private Arrow arrow;
    private Box textBox;

    private GraphicLink graphicLink;
    private ModelLink modelLink;

    private State state;

    private bool hovered;// = false;

    private int opacity;

    public String Tag
    {
      get { return graphicLink.Tag; }
    }

    public int Opacity
    {
      get { return opacity; }
      //set { opacity = value; }
    }
    private Timer opacityTimer = new Timer();

    //private System.IO.StreamWriter debugLog;

    public EditorLink(State state, GraphicLink graphicLink, ModelLink modelLink)
    {
      this.state = state;
      this.graphicLink = graphicLink;
      this.modelLink = modelLink;

      opacityTimer.Interval = 100;
      opacityTimer.Elapsed += new ElapsedEventHandler(opacityTimer_Elapsed);

      //debugLog = new System.IO.StreamWriter("c:\\" + guid.ToString() + " - " + tag + ".log");
    }

    private void opacityTimer_Elapsed(object source, ElapsedEventArgs e)
    {
      if (Visible && ((hovered) || ((arrow.Origin.Tag as EditorNode).Hovered) || ((arrow.Destination.Tag as EditorNode).Hovered)))
      {
        if (opacity == 220)
        {
          opacityTimer.Stop();
          return;
        }

        opacity += 50;
        if (opacity > 220)
        {
          opacityTimer.Stop();
          opacity = 220;
          Console.WriteLine("Shown: " + Tag);
        }
      }
      else
      {
        if (opacity == 1)
        {
          opacityTimer.Stop();
          return;
        }

        opacity -= 50;
        if (opacity < 1)
        {
          opacityTimer.Stop();
          opacity = 1;
          Console.WriteLine("Hidden:" + Tag);
        }
      }

      // Play with fillcolor to cause invalidate to be called on each object.
      (arrow.Origin.Tag as EditorNode).ModelBox.FillColor = (arrow.Origin.Tag as EditorNode).ModelBox.FillColor;
      (arrow.Destination.Tag as EditorNode).ModelBox.FillColor = (arrow.Destination.Tag as EditorNode).ModelBox.FillColor;
      
      Color fillColor = arrow.FillColor;
      arrow.FillColor = Color.Black;
      arrow.FillColor = fillColor;
    }

    internal void Remove(FlowChart flowChart)
    {

      if (Arrow != null)
        flowChart.DeleteObject(Arrow);
    }

    public Arrow Arrow
    {
      get { 
        //DebugCheck(); 
        return arrow; 
      }
      set { 
        //DebugCheck(); 
        arrow = value; 
      }
    }

    //private void DebugCheck()
    //{
    //  if (arrow != null)
    //  {
    //    debugLog.WriteLine(arrow.ControlPoints.ToString());

    //    System.Diagnostics.StackFrame stackFrame = new System.Diagnostics.StackFrame(1, true);

    //    debugLog.WriteLine(stackFrame.ToString());

    //    for (int i = 1; i < arrow.ControlPoints.Count; i++)
    //    {
    //      if ((arrow.ControlPoints[i - 1].X != arrow.ControlPoints[i].X) && (arrow.ControlPoints[i - 1].Y != arrow.ControlPoints[i].Y))
    //        System.Diagnostics.Debugger.Break();
    //    }
    //  }
    //}

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
      get { return graphicLink.Guid; }
    }

    //public String Tag
    //{
    //  get { return tag; }
    //}

    public bool Hovered
    {
      get { return hovered; }
      set
      { 
        hovered = value;

        if (arrow.Origin is Box)
          (arrow.Origin.Tag as EditorNode).UpdateVisibility();
        if (arrow.Destination is Box)
          (arrow.Destination.Tag as EditorNode).UpdateVisibility();
        
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
      arrow.ZIndex = Math.Max(arrow.Origin.ZIndex, arrow.Destination.ZIndex) + 10000;
      textBox.ZIndex = arrow.ZIndex - 10000;


//      arrow.CustomDraw = CustomDraw.Additional;

      if ((hovered) || ((arrow.Origin.Tag as EditorNode).Hovered) || ((arrow.Destination.Tag as EditorNode).Hovered))
        arrow.ZTop();

      opacityTimer.Start();
    }
  }
}
