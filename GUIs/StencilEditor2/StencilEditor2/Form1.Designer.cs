namespace StencilEditor2
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
      this.statusStrip1 = new System.Windows.Forms.StatusStrip();
      this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
      this.menuStrip1 = new System.Windows.Forms.MenuStrip();
      this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
      this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
      this.stencilModeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.modelStencilToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.graphicStencilToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.editPropertiesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.resetZoomToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.inToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.outToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.resetToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.gridToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem3 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem5 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStripMenuItem6 = new System.Windows.Forms.ToolStripMenuItem();
      this.toolStrip1 = new System.Windows.Forms.ToolStrip();
      this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
      this.toolStripButton2 = new System.Windows.Forms.ToolStripButton();
      this.statusStrip1.SuspendLayout();
      this.menuStrip1.SuspendLayout();
      this.toolStrip1.SuspendLayout();
      this.SuspendLayout();
      // 
      // flowChart1
      // 
      this.flowChart1.ActiveMnpColor = System.Drawing.Color.DodgerBlue;
      this.flowChart1.AllowUnconnectedArrows = true;
      this.flowChart1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.flowChart1.AntiAlias = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
      this.flowChart1.ArrowBase = MindFusion.FlowChartX.ArrowHead.Rhombus;
      this.flowChart1.ArrowBaseSize = 1F;
      this.flowChart1.ArrowHead = MindFusion.FlowChartX.ArrowHead.Rhombus;
      this.flowChart1.ArrowHeadSize = 1F;
      this.flowChart1.ArrowIntermSize = 1F;
      this.flowChart1.BackBrush = new MindFusion.FlowChartX.SolidBrush("#FFFFFFC0");
      this.flowChart1.Behavior = MindFusion.FlowChartX.BehaviorType.Modify;
      this.flowChart1.CurrentCursor = System.Windows.Forms.Cursors.Default;
      this.flowChart1.Cursor = System.Windows.Forms.Cursors.Default;
      this.flowChart1.DefaultControlType = typeof(System.Windows.Forms.Button);
      this.flowChart1.DocExtents = ((System.Drawing.RectangleF)(resources.GetObject("flowChart1.DocExtents")));
      this.flowChart1.DoubleBuffer = true;
      this.flowChart1.Enabled = false;
      this.flowChart1.GridSizeX = 1F;
      this.flowChart1.GridSizeY = 1F;
      this.flowChart1.InplaceEditFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.flowChart1.Location = new System.Drawing.Point(0, 52);
      this.flowChart1.Name = "flowChart1";
      this.flowChart1.RestrObjsToDoc = MindFusion.FlowChartX.RestrictToDoc.InsideOnly;
      this.flowChart1.SelHandleSize = 1F;
      this.flowChart1.ShadowsStyle = MindFusion.FlowChartX.ShadowsStyle.None;
      this.flowChart1.ShowAnchors = MindFusion.FlowChartX.ShowAnchors.Never;
      this.flowChart1.ShowDisabledHandles = false;
      this.flowChart1.Size = new System.Drawing.Size(825, 704);
      this.flowChart1.TabIndex = 0;
      this.flowChart1.BoxModified += new MindFusion.FlowChartX.BoxMouseEvent(this.flowChart1_BoxModified);
      this.flowChart1.ArrowModified += new MindFusion.FlowChartX.ArrowMouseEvent(this.flowChart1_ArrowModified);
      this.flowChart1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.flowChart1_MouseClick);
      this.flowChart1.ArrowModifying += new MindFusion.FlowChartX.ArrowConfirmation(this.flowChart1_ArrowModifying);
      this.flowChart1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.flowChart1_MouseMove);
      // 
      // statusStrip1
      // 
      this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1});
      this.statusStrip1.Location = new System.Drawing.Point(0, 759);
      this.statusStrip1.Name = "statusStrip1";
      this.statusStrip1.Size = new System.Drawing.Size(825, 22);
      this.statusStrip1.TabIndex = 1;
      this.statusStrip1.Text = "statusStrip1";
      // 
      // toolStripStatusLabel1
      // 
      this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
      this.toolStripStatusLabel1.Size = new System.Drawing.Size(0, 17);
      // 
      // menuStrip1
      // 
      this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.viewToolStripMenuItem});
      this.menuStrip1.Location = new System.Drawing.Point(0, 0);
      this.menuStrip1.Name = "menuStrip1";
      this.menuStrip1.Size = new System.Drawing.Size(825, 24);
      this.menuStrip1.TabIndex = 2;
      this.menuStrip1.Text = "menuStrip1";
      // 
      // toolStripMenuItem1
      // 
      this.toolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.toolStripSeparator1,
            this.stencilModeToolStripMenuItem,
            this.editPropertiesToolStripMenuItem});
      this.toolStripMenuItem1.Name = "toolStripMenuItem1";
      this.toolStripMenuItem1.Size = new System.Drawing.Size(35, 20);
      this.toolStripMenuItem1.Text = "&File";
      // 
      // openToolStripMenuItem
      // 
      this.openToolStripMenuItem.Name = "openToolStripMenuItem";
      this.openToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
      this.openToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.openToolStripMenuItem.Text = "&Open...";
      this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
      // 
      // toolStripSeparator1
      // 
      this.toolStripSeparator1.Name = "toolStripSeparator1";
      this.toolStripSeparator1.Size = new System.Drawing.Size(149, 6);
      // 
      // stencilModeToolStripMenuItem
      // 
      this.stencilModeToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.modelStencilToolStripMenuItem,
            this.graphicStencilToolStripMenuItem});
      this.stencilModeToolStripMenuItem.Name = "stencilModeToolStripMenuItem";
      this.stencilModeToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.stencilModeToolStripMenuItem.Text = "Stencil Mode";
      // 
      // modelStencilToolStripMenuItem
      // 
      this.modelStencilToolStripMenuItem.Checked = true;
      this.modelStencilToolStripMenuItem.CheckOnClick = true;
      this.modelStencilToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
      this.modelStencilToolStripMenuItem.Enabled = false;
      this.modelStencilToolStripMenuItem.Name = "modelStencilToolStripMenuItem";
      this.modelStencilToolStripMenuItem.Size = new System.Drawing.Size(141, 22);
      this.modelStencilToolStripMenuItem.Text = "ModelStencil";
      // 
      // graphicStencilToolStripMenuItem
      // 
      this.graphicStencilToolStripMenuItem.Enabled = false;
      this.graphicStencilToolStripMenuItem.Name = "graphicStencilToolStripMenuItem";
      this.graphicStencilToolStripMenuItem.Size = new System.Drawing.Size(141, 22);
      this.graphicStencilToolStripMenuItem.Text = "GraphicStencil";
      // 
      // editPropertiesToolStripMenuItem
      // 
      this.editPropertiesToolStripMenuItem.Enabled = false;
      this.editPropertiesToolStripMenuItem.Name = "editPropertiesToolStripMenuItem";
      this.editPropertiesToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
      this.editPropertiesToolStripMenuItem.Text = "Edit Properties";
      this.editPropertiesToolStripMenuItem.Click += new System.EventHandler(this.editPropertiesToolStripMenuItem_Click);
      // 
      // viewToolStripMenuItem
      // 
      this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.resetZoomToolStripMenuItem,
            this.gridToolStripMenuItem});
      this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
      this.viewToolStripMenuItem.Size = new System.Drawing.Size(41, 20);
      this.viewToolStripMenuItem.Text = "&View";
      // 
      // resetZoomToolStripMenuItem
      // 
      this.resetZoomToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.inToolStripMenuItem,
            this.outToolStripMenuItem,
            this.resetToolStripMenuItem});
      this.resetZoomToolStripMenuItem.Name = "resetZoomToolStripMenuItem";
      this.resetZoomToolStripMenuItem.Size = new System.Drawing.Size(115, 22);
      this.resetZoomToolStripMenuItem.Text = "&Zoom";
      // 
      // inToolStripMenuItem
      // 
      this.inToolStripMenuItem.Name = "inToolStripMenuItem";
      this.inToolStripMenuItem.ShortcutKeyDisplayString = "Ctrl++";
      this.inToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Oemplus)));
      this.inToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
      this.inToolStripMenuItem.Text = "In";
      this.inToolStripMenuItem.Click += new System.EventHandler(this.inToolStripMenuItem_Click);
      // 
      // outToolStripMenuItem
      // 
      this.outToolStripMenuItem.Name = "outToolStripMenuItem";
      this.outToolStripMenuItem.ShortcutKeyDisplayString = "Ctrl+-";
      this.outToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.OemMinus)));
      this.outToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
      this.outToolStripMenuItem.Text = "Out";
      this.outToolStripMenuItem.Click += new System.EventHandler(this.outToolStripMenuItem_Click);
      // 
      // resetToolStripMenuItem
      // 
      this.resetToolStripMenuItem.Name = "resetToolStripMenuItem";
      this.resetToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.D0)));
      this.resetToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
      this.resetToolStripMenuItem.Text = "Reset";
      this.resetToolStripMenuItem.Click += new System.EventHandler(this.resetToolStripMenuItem_Click);
      // 
      // gridToolStripMenuItem
      // 
      this.gridToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem3,
            this.toolStripMenuItem4,
            this.toolStripMenuItem5,
            this.toolStripMenuItem6});
      this.gridToolStripMenuItem.Name = "gridToolStripMenuItem";
      this.gridToolStripMenuItem.Size = new System.Drawing.Size(115, 22);
      this.gridToolStripMenuItem.Text = "Grid Size";
      // 
      // toolStripMenuItem3
      // 
      this.toolStripMenuItem3.Checked = true;
      this.toolStripMenuItem3.CheckState = System.Windows.Forms.CheckState.Checked;
      this.toolStripMenuItem3.Name = "toolStripMenuItem3";
      this.toolStripMenuItem3.Size = new System.Drawing.Size(97, 22);
      this.toolStripMenuItem3.Text = "1%";
      this.toolStripMenuItem3.Click += new System.EventHandler(this.toolStripMenuItem3_Click);
      // 
      // toolStripMenuItem4
      // 
      this.toolStripMenuItem4.Name = "toolStripMenuItem4";
      this.toolStripMenuItem4.Size = new System.Drawing.Size(97, 22);
      this.toolStripMenuItem4.Text = "2%";
      this.toolStripMenuItem4.Click += new System.EventHandler(this.toolStripMenuItem4_Click);
      // 
      // toolStripMenuItem5
      // 
      this.toolStripMenuItem5.Name = "toolStripMenuItem5";
      this.toolStripMenuItem5.Size = new System.Drawing.Size(97, 22);
      this.toolStripMenuItem5.Text = "5%";
      this.toolStripMenuItem5.Click += new System.EventHandler(this.toolStripMenuItem5_Click);
      // 
      // toolStripMenuItem6
      // 
      this.toolStripMenuItem6.Name = "toolStripMenuItem6";
      this.toolStripMenuItem6.Size = new System.Drawing.Size(97, 22);
      this.toolStripMenuItem6.Text = "10%";
      this.toolStripMenuItem6.Click += new System.EventHandler(this.toolStripMenuItem6_Click);
      // 
      // toolStrip1
      // 
      this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.toolStripButton2});
      this.toolStrip1.Location = new System.Drawing.Point(0, 24);
      this.toolStrip1.Name = "toolStrip1";
      this.toolStrip1.Size = new System.Drawing.Size(825, 25);
      this.toolStrip1.TabIndex = 3;
      this.toolStrip1.Text = "toolStrip1";
      // 
      // toolStripButton1
      // 
      this.toolStripButton1.Checked = true;
      this.toolStripButton1.CheckOnClick = true;
      this.toolStripButton1.CheckState = System.Windows.Forms.CheckState.Checked;
      this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
      this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton1.Name = "toolStripButton1";
      this.toolStripButton1.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton1.Text = "toolStripButton1";
      this.toolStripButton1.ToolTipText = "Toggle Previews";
      this.toolStripButton1.CheckStateChanged += new System.EventHandler(this.toolStripButton1_CheckStateChanged);
      // 
      // toolStripButton2
      // 
      this.toolStripButton2.Checked = true;
      this.toolStripButton2.CheckOnClick = true;
      this.toolStripButton2.CheckState = System.Windows.Forms.CheckState.Checked;
      this.toolStripButton2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.toolStripButton2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton2.Image")));
      this.toolStripButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.toolStripButton2.Name = "toolStripButton2";
      this.toolStripButton2.Size = new System.Drawing.Size(23, 22);
      this.toolStripButton2.Text = "toolStripButton2";
      this.toolStripButton2.ToolTipText = "Toggle Grid";
      this.toolStripButton2.CheckStateChanged += new System.EventHandler(this.toolStripButton2_CheckStateChanged);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(825, 781);
      this.Controls.Add(this.toolStrip1);
      this.Controls.Add(this.statusStrip1);
      this.Controls.Add(this.menuStrip1);
      this.Controls.Add(this.flowChart1);
      this.MainMenuStrip = this.menuStrip1;
      this.Name = "Form1";
      this.Text = "Form1";
      this.Activated += new System.EventHandler(this.Form1_Activated);
      this.VisibleChanged += new System.EventHandler(this.Form1_VisibleChanged);
      this.Move += new System.EventHandler(this.Form1_Move);
      this.Resize += new System.EventHandler(this.Form1_Resize);
      this.statusStrip1.ResumeLayout(false);
      this.statusStrip1.PerformLayout();
      this.menuStrip1.ResumeLayout(false);
      this.menuStrip1.PerformLayout();
      this.toolStrip1.ResumeLayout(false);
      this.toolStrip1.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private MindFusion.FlowChartX.FlowChart flowChart1;
    private System.Windows.Forms.StatusStrip statusStrip1;
    private System.Windows.Forms.MenuStrip menuStrip1;
    private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
    private System.Windows.Forms.ToolStrip toolStrip1;
    private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
    private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
    private System.Windows.Forms.ToolStripButton toolStripButton1;
    private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem resetZoomToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem inToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem outToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem resetToolStripMenuItem;
    private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
    private System.Windows.Forms.ToolStripButton toolStripButton2;
    private System.Windows.Forms.ToolStripMenuItem gridToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem3;
    private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem4;
    private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem5;
    private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem6;
    private System.Windows.Forms.ToolStripMenuItem stencilModeToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem modelStencilToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem graphicStencilToolStripMenuItem;
    private System.Windows.Forms.ToolStripMenuItem editPropertiesToolStripMenuItem;

  }
}

