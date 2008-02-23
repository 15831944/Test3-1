namespace StencilEditor2
{
  partial class Preview
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Preview));
      this.flowChart1 = new MindFusion.FlowChartX.FlowChart();
      this.SuspendLayout();
      // 
      // flowChart1
      // 
      this.flowChart1.AlignToGrid = false;
      this.flowChart1.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
      this.flowChart1.CurrentCursor = System.Windows.Forms.Cursors.Default;
      this.flowChart1.Cursor = System.Windows.Forms.Cursors.Default;
      this.flowChart1.DefaultControlType = typeof(System.Windows.Forms.Button);
      this.flowChart1.DocExtents = ((System.Drawing.RectangleF)(resources.GetObject("flowChart1.DocExtents")));
      this.flowChart1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowChart1.DoubleBuffer = true;
      this.flowChart1.Enabled = false;
      this.flowChart1.InplaceEditFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.flowChart1.Location = new System.Drawing.Point(0, 0);
      this.flowChart1.Name = "flowChart1";
      this.flowChart1.ShadowsStyle = MindFusion.FlowChartX.ShadowsStyle.None;
      this.flowChart1.ShowAnchors = MindFusion.FlowChartX.ShowAnchors.Always;
      this.flowChart1.ShowDisabledHandles = false;
      this.flowChart1.ShowScrollbars = false;
      this.flowChart1.Size = this.ClientSize;
      this.flowChart1.TabIndex = 0;
      // 
      // Preview
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.ClientSize = new System.Drawing.Size(210, 210);
      this.ControlBox = false;
      this.Controls.Add(this.flowChart1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "Preview";
      this.ShowIcon = false;
      this.ShowInTaskbar = false;
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
      this.Text = "Preview";
      this.Activated += new System.EventHandler(this.Preview_Activated);
      this.ResumeLayout(false);

    }

    #endregion

    private MindFusion.FlowChartX.FlowChart flowChart1;
  }
}