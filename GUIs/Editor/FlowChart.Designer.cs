using MindFusion.FlowChartX;
namespace SysCAD.Editor
{
  partial class FrmFlowChart
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmFlowChart));
      this.fcFlowChart = new MindFusion.FlowChartX.FlowChart();
      this.SuspendLayout();
      // 
      // fcFlowChart
      // 
      this.fcFlowChart.AllowDrop = true;
      this.fcFlowChart.AllowRefLinks = false;
      this.fcFlowChart.AllowUnconnectedArrows = true;
      this.fcFlowChart.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
      this.fcFlowChart.ArrowBaseSize = 2.5F;
      this.fcFlowChart.ArrowBrush = new MindFusion.FlowChartX.SolidBrush("#FF000000");
      this.fcFlowChart.ArrowCrossings = MindFusion.FlowChartX.ArrowCrossings.BreakOff;
      this.fcFlowChart.ArrowHandlesStyle = MindFusion.FlowChartX.HandlesStyle.SquareHandles2;
      this.fcFlowChart.ArrowHead = MindFusion.FlowChartX.ArrowHead.None;
      this.fcFlowChart.ArrowHeadSize = 2.5F;
      this.fcFlowChart.ArrowIntermSize = 1.5F;
      this.fcFlowChart.ArrowSegments = ((short)(2));
      this.fcFlowChart.ArrowsSnapToBorders = true;
      this.fcFlowChart.ArrowsSplittable = true;
      this.fcFlowChart.ArrowStyle = MindFusion.FlowChartX.ArrowStyle.Cascading;
      this.fcFlowChart.AutoSizeDoc = MindFusion.FlowChartX.AutoSize.AllDirections;
      this.fcFlowChart.Behavior = MindFusion.FlowChartX.BehaviorType.Modify;
      this.fcFlowChart.BoxCustomDraw = MindFusion.FlowChartX.CustomDraw.Additional2;
      this.fcFlowChart.BoxHandlesStyle = MindFusion.FlowChartX.HandlesStyle.HatchHandles2;
      this.fcFlowChart.BoxStyle = MindFusion.FlowChartX.BoxStyle.Shape;
      this.fcFlowChart.DefaultControlType = typeof(System.Windows.Forms.Button);
      this.fcFlowChart.DocExtents = ((System.Drawing.RectangleF)(resources.GetObject("fcFlowChart.DocExtents")));
      this.fcFlowChart.Dock = System.Windows.Forms.DockStyle.Fill;
      this.fcFlowChart.Font = new System.Drawing.Font("Microsoft Sans Serif", 5.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.fcFlowChart.GridSizeX = 2F;
      this.fcFlowChart.GridSizeY = 2F;
      this.fcFlowChart.InplaceEditFont = new System.Drawing.Font("Microsoft Sans Serif", 5.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.fcFlowChart.IntermArrowHead = MindFusion.FlowChartX.ArrowHead.Triangle;
      this.fcFlowChart.Location = new System.Drawing.Point(0, 0);
      this.fcFlowChart.Margin = new System.Windows.Forms.Padding(2);
      this.fcFlowChart.Name = "fcFlowChart";
      this.fcFlowChart.PrintOptions.EnableShadows = false;
      this.fcFlowChart.RestrObjsToDoc = MindFusion.FlowChartX.RestrictToDoc.NoRestriction;
      this.fcFlowChart.SelHandleSize = 1.5F;
      this.fcFlowChart.ShadowsStyle = MindFusion.FlowChartX.ShadowsStyle.None;
      this.fcFlowChart.Size = new System.Drawing.Size(853, 460);
      this.fcFlowChart.TabIndex = 0;
      this.fcFlowChart.ArrowModified += new MindFusion.FlowChartX.ArrowMouseEvent(this.fcFlowChart_ArrowModified);
      this.fcFlowChart.ArrowDeleted += new MindFusion.FlowChartX.ArrowEvent(this.fcFlowChart_ArrowDeleted);
      this.fcFlowChart.MouseMove += new System.Windows.Forms.MouseEventHandler(this.fcFlowChart_MouseMove);
      this.fcFlowChart.BoxModified += new MindFusion.FlowChartX.BoxMouseEvent(this.fcFlowChart_BoxModified);
      this.fcFlowChart.BoxCreated += new MindFusion.FlowChartX.BoxEvent(this.fcFlowChart_BoxCreated);
      this.fcFlowChart.BoxModifying += new MindFusion.FlowChartX.BoxMouseEvent(this.fcFlowChart_BoxModifying);
      this.fcFlowChart.BoxDeleted += new MindFusion.FlowChartX.BoxEvent(this.fcFlowChart_BoxDeleted);
      this.fcFlowChart.DrawBox += new MindFusion.FlowChartX.BoxCustomDraw(this.fcFlowChart_DrawBox);
      this.fcFlowChart.BoxCreating += new MindFusion.FlowChartX.BoxConfirmation(this.fcFlowChart_BoxCreating);
      this.fcFlowChart.ArrowCreated += new MindFusion.FlowChartX.ArrowEvent(this.fcFlowChart_ArrowCreated);
      this.fcFlowChart.ArrowModifying += new MindFusion.FlowChartX.ArrowMouseEvent(this.fcFlowChart_ArrowModifying);
      this.fcFlowChart.Click += new System.EventHandler(this.fcFlowChart_Click);
      // 
      // FrmFlowChart
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(853, 460);
      this.Controls.Add(this.fcFlowChart);
      this.Margin = new System.Windows.Forms.Padding(2);
      this.Name = "FrmFlowChart";
      this.Text = "FlowChart";
      this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
      this.Resize += new System.EventHandler(this.FrmFlowChart_Resize);
      this.ResumeLayout(false);

    }

    #endregion

    public FlowChart fcFlowChart;
  }
}