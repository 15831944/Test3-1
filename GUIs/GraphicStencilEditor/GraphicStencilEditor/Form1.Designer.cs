namespace StencilEditor
{
  partial class Form1
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      this.flowChart1 = new MindFusion.FlowChartX.FlowChart();
      this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
      this.toolStrip1 = new System.Windows.Forms.ToolStrip();
      this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton2 = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton3 = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton4 = new System.Windows.Forms.ToolStripButton();
      this.toolStripContainer1.ContentPanel.SuspendLayout();
      this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
      this.toolStripContainer1.SuspendLayout();
      this.toolStrip1.SuspendLayout();
      this.SuspendLayout();
      // 
      // flowChart1
      // 
      this.flowChart1.AllowUnconnectedArrows = true;
      this.flowChart1.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
      this.flowChart1.ArrowCustomDraw = MindFusion.FlowChartX.CustomDraw.ShadowOnly;
      this.flowChart1.ArrowHead = MindFusion.FlowChartX.ArrowHead.None;
      this.flowChart1.Behavior = MindFusion.FlowChartX.BehaviorType.Modify;
      this.flowChart1.BoxCustomDraw = MindFusion.FlowChartX.CustomDraw.Full;
      this.flowChart1.DefaultControlType = typeof(System.Windows.Forms.Button);
      this.flowChart1.DocExtents = ((System.Drawing.RectangleF)(resources.GetObject("flowChart1.DocExtents")));
      this.flowChart1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowChart1.DoubleBuffer = true;
      this.flowChart1.GridSizeX = 5F;
      this.flowChart1.GridSizeY = 5F;
      this.flowChart1.GridStyle = MindFusion.FlowChartX.GridStyle.Lines;
      this.flowChart1.InplaceEditFont = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.flowChart1.Location = new System.Drawing.Point(0, 0);
      this.flowChart1.Name = "flowChart1";
      this.flowChart1.ShowGrid = true;
      this.flowChart1.Size = new System.Drawing.Size(964, 625);
      this.flowChart1.TabIndex = 0;
      this.flowChart1.ArrowModified += new MindFusion.FlowChartX.ArrowMouseEvent(this.flowChart1_ArrowModified);
      this.flowChart1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.flowChart1_MouseClick);
      this.flowChart1.ArrowModifying += new MindFusion.FlowChartX.ArrowMouseEvent(this.flowChart1_ArrowModifying);
      // 
      // toolStripContainer1
      // 
      this.toolStripContainer1.BottomToolStripPanelVisible = false;
      // 
      // toolStripContainer1.ContentPanel
      // 
      this.toolStripContainer1.ContentPanel.AutoScroll = true;
      this.toolStripContainer1.ContentPanel.Controls.Add(this.flowChart1);
      this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(964, 625);
      this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.toolStripContainer1.LeftToolStripPanelVisible = false;
      this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
      this.toolStripContainer1.Name = "toolStripContainer1";
      this.toolStripContainer1.RightToolStripPanelVisible = false;
      this.toolStripContainer1.Size = new System.Drawing.Size(964, 650);
      this.toolStripContainer1.TabIndex = 2;
      this.toolStripContainer1.Text = "toolStripContainer1";
      // 
      // toolStripContainer1.TopToolStripPanel
      // 
      this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolStrip1);
      // 
      // toolStrip1
      // 
      this.toolStrip1.Dock = System.Windows.Forms.DockStyle.None;
      this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.toolStripButton2,
            this.toolStripButton3,
            this.toolStripButton4});
      this.toolStrip1.Location = new System.Drawing.Point(3, 0);
      this.toolStrip1.Name = "toolStrip1";
      this.toolStrip1.Size = new System.Drawing.Size(628, 25);
      this.toolStrip1.TabIndex = 2;
      // 
      // toolStripButton1
      // 
      this.toolStripButton1.Checked = true;
      this.toolStripButton1.CheckOnClick = true;
      this.toolStripButton1.CheckState = System.Windows.Forms.CheckState.Checked;
      this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
      this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
      this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton1.Name = "toolStripButton1";
      this.toolStripButton1.Size = new System.Drawing.Size(132, 22);
      this.toolStripButton1.Text = "Show FillArea Lines";
      this.toolStripButton1.Click += new System.EventHandler(this.toolStripButton1_Click);
      // 
      // toolStripButton2
      // 
      this.toolStripButton2.Checked = true;
      this.toolStripButton2.CheckOnClick = true;
      this.toolStripButton2.CheckState = System.Windows.Forms.CheckState.Checked;
      this.toolStripButton2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
      this.toolStripButton2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton2.Image")));
      this.toolStripButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton2.Name = "toolStripButton2";
      this.toolStripButton2.Size = new System.Drawing.Size(157, 22);
      this.toolStripButton2.Text = "Show Decoration Lines";
      this.toolStripButton2.Click += new System.EventHandler(this.toolStripButton2_Click);
      // 
      // toolStripButton3
      // 
      this.toolStripButton3.Checked = true;
      this.toolStripButton3.CheckOnClick = true;
      this.toolStripButton3.CheckState = System.Windows.Forms.CheckState.Checked;
      this.toolStripButton3.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
      this.toolStripButton3.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton3.Image")));
      this.toolStripButton3.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton3.Name = "toolStripButton3";
      this.toolStripButton3.Size = new System.Drawing.Size(143, 22);
      this.toolStripButton3.Text = "Show Sample Stencil";
      this.toolStripButton3.Click += new System.EventHandler(this.toolStripButton3_Click);
      // 
      // toolStripButton4
      // 
      this.toolStripButton4.Checked = true;
      this.toolStripButton4.CheckOnClick = true;
      this.toolStripButton4.CheckState = System.Windows.Forms.CheckState.Checked;
      this.toolStripButton4.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
      this.toolStripButton4.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton4.Image")));
      this.toolStripButton4.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton4.Name = "toolStripButton4";
      this.toolStripButton4.Size = new System.Drawing.Size(153, 22);
      this.toolStripButton4.Text = "Show Canvas Extents";
      this.toolStripButton4.Click += new System.EventHandler(this.toolStripButton4_Click);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(964, 650);
      this.Controls.Add(this.toolStripContainer1);
      this.Name = "Form1";
      this.Text = "Form1";
      this.toolStripContainer1.ContentPanel.ResumeLayout(false);
      this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
      this.toolStripContainer1.TopToolStripPanel.PerformLayout();
      this.toolStripContainer1.ResumeLayout(false);
      this.toolStripContainer1.PerformLayout();
      this.toolStrip1.ResumeLayout(false);
      this.toolStrip1.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private MindFusion.FlowChartX.FlowChart flowChart1;
    private System.Windows.Forms.ToolStripContainer toolStripContainer1;
    private System.Windows.Forms.ToolStrip toolStrip1;
    private System.Windows.Forms.ToolStripButton toolStripButton1;
    private System.Windows.Forms.ToolStripButton toolStripButton2;
    private System.Windows.Forms.ToolStripButton toolStripButton3;
    private System.Windows.Forms.ToolStripButton toolStripButton4;



  }
}

