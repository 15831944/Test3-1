using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MindFusion.FlowChartX;
using SysCAD.Protocol;

namespace StencilEditor2
{
  public partial class Preview : Form
  {
    Form1 mainForm;
    ToolStripMenuItem toolStripMenuItem;
    int size;
    int pos;

    Box modelBox;

    public Preview(Form1 mainForm, ToolStripMenuItem toolStripMenuItem, int size, int pos)
    {
      this.mainForm = mainForm;
      this.toolStripMenuItem = toolStripMenuItem;
      this.size = size;
      this.pos = pos;

      InitializeComponent();

      this.flowChart1.DocExtents = new RectangleF(-10.0F, -10.0F, 120.0F, 120.0F);

      Box backBox = flowChart1.CreateBox(0.0F, 0.0F, 100.0F, 100.0F);
      backBox.Style = BoxStyle.Rectangle;
      backBox.FillColor = System.Drawing.Color.FromArgb(55, System.Drawing.Color.LightSlateGray);
      backBox.FrameColor = System.Drawing.Color.FromArgb(61, System.Drawing.Color.LightSlateGray);
      backBox.Locked = true;
    }
    
    internal void UpdatePreview()
    {
      Left = mainForm.RectangleToScreen(mainForm.ClientRectangle).Right + 4;
      Top = mainForm.RectangleToScreen(mainForm.ClientRectangle).Top - 24 + pos;
      Width = size ;
      Height = size;
      Visible = toolStripMenuItem.Checked && mainForm.Visible && (!(mainForm.WindowState == FormWindowState.Minimized));

      this.flowChart1.Size = ClientSize;
      this.flowChart1.ZoomToRect(this.flowChart1.DocExtents);
    }

    private void Preview_Activated(object sender, EventArgs e)
    {
      mainForm.Activate();
    }

    internal void SetStencil(ShapeTemplate template)
    {
      flowChart1.ClearAll();

      Box backBox = flowChart1.CreateBox(0.0F, 0.0F, 100.0F, 100.0F);
      backBox.Style = BoxStyle.Rectangle;
      backBox.FillColor = System.Drawing.Color.FromArgb(55, System.Drawing.Color.LightSlateGray);
      backBox.FrameColor = System.Drawing.Color.FromArgb(61, System.Drawing.Color.LightSlateGray);
      backBox.Locked = true;

      modelBox = flowChart1.CreateBox(0.0F, 0.0F, 100.0F, 100.0F);
      modelBox.RotationAngle = 0.0F;
      modelBox.Style = BoxStyle.Shape;
      modelBox.Shape = template;
      modelBox.FillColor = System.Drawing.Color.FromArgb(220, 222, 184, 136);
      modelBox.FrameColor = System.Drawing.Color.FromArgb(255, 111, 92, 68);
      modelBox.EnabledHandles = Handles.None;
      modelBox.HandlesStyle = HandlesStyle.Invisible;

    }
  }
}
