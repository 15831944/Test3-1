namespace SysCAD
{
  partial class StencilChooser
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

    #region Component Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.modelTreeView = new System.Windows.Forms.TreeView();
      this.currentPanel = new System.Windows.Forms.Panel();
      this.currentLabel = new System.Windows.Forms.Label();
      this.currentGraphiclPictureBox = new System.Windows.Forms.PictureBox();
      this.currentModelPictureBox = new System.Windows.Forms.PictureBox();
      this.globalSplitContainer = new System.Windows.Forms.SplitContainer();
      this.treeViewSplitContainer = new System.Windows.Forms.SplitContainer();
      this.graphicTreeView = new System.Windows.Forms.TreeView();
      this.mruSplitContainer = new System.Windows.Forms.SplitContainer();
      this.mruListBox = new System.Windows.Forms.ListView();
      this.itemColumnHeader = new System.Windows.Forms.ColumnHeader();
      this.currentPanel.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.currentGraphiclPictureBox)).BeginInit();
      ((System.ComponentModel.ISupportInitialize)(this.currentModelPictureBox)).BeginInit();
      this.globalSplitContainer.Panel1.SuspendLayout();
      this.globalSplitContainer.Panel2.SuspendLayout();
      this.globalSplitContainer.SuspendLayout();
      this.treeViewSplitContainer.Panel1.SuspendLayout();
      this.treeViewSplitContainer.Panel2.SuspendLayout();
      this.treeViewSplitContainer.SuspendLayout();
      this.mruSplitContainer.Panel1.SuspendLayout();
      this.mruSplitContainer.Panel2.SuspendLayout();
      this.mruSplitContainer.SuspendLayout();
      this.SuspendLayout();
      // 
      // modelTreeView
      // 
      this.modelTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.modelTreeView.Location = new System.Drawing.Point(0, 0);
      this.modelTreeView.Name = "modelTreeView";
      this.modelTreeView.Size = new System.Drawing.Size(150, 184);
      this.modelTreeView.TabIndex = 2;
      this.modelTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.modelTreeView_AfterSelect);
      this.modelTreeView.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.modelTreeView_BeforeSelect);
      // 
      // currentPanel
      // 
      this.currentPanel.AutoSize = true;
      this.currentPanel.BackColor = System.Drawing.SystemColors.Window;
      this.currentPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.currentPanel.Controls.Add(this.currentLabel);
      this.currentPanel.Controls.Add(this.currentGraphiclPictureBox);
      this.currentPanel.Controls.Add(this.currentModelPictureBox);
      this.currentPanel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.currentPanel.Location = new System.Drawing.Point(0, 0);
      this.currentPanel.Name = "currentPanel";
      this.currentPanel.Size = new System.Drawing.Size(150, 22);
      this.currentPanel.TabIndex = 3;
      this.currentPanel.Click += new System.EventHandler(this.panel1_Click);
      // 
      // currentLabel
      // 
      this.currentLabel.AutoSize = true;
      this.currentLabel.Location = new System.Drawing.Point(38, 2);
      this.currentLabel.Name = "currentLabel";
      this.currentLabel.Size = new System.Drawing.Size(0, 13);
      this.currentLabel.TabIndex = 2;
      this.currentLabel.Click += new System.EventHandler(this.label1_Click);
      // 
      // currentGraphiclPictureBox
      // 
      this.currentGraphiclPictureBox.Location = new System.Drawing.Point(17, 0);
      this.currentGraphiclPictureBox.Name = "currentGraphiclPictureBox";
      this.currentGraphiclPictureBox.Size = new System.Drawing.Size(16, 16);
      this.currentGraphiclPictureBox.TabIndex = 1;
      this.currentGraphiclPictureBox.TabStop = false;
      this.currentGraphiclPictureBox.Visible = false;
      this.currentGraphiclPictureBox.Click += new System.EventHandler(this.pictureBox2_Click);
      // 
      // currentModelPictureBox
      // 
      this.currentModelPictureBox.Location = new System.Drawing.Point(0, 0);
      this.currentModelPictureBox.Name = "currentModelPictureBox";
      this.currentModelPictureBox.Size = new System.Drawing.Size(16, 16);
      this.currentModelPictureBox.TabIndex = 0;
      this.currentModelPictureBox.TabStop = false;
      this.currentModelPictureBox.Visible = false;
      this.currentModelPictureBox.Click += new System.EventHandler(this.pictureBox1_Click);
      // 
      // globalSplitContainer
      // 
      this.globalSplitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
      this.globalSplitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
      this.globalSplitContainer.Location = new System.Drawing.Point(0, 0);
      this.globalSplitContainer.Name = "globalSplitContainer";
      this.globalSplitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // globalSplitContainer.Panel1
      // 
      this.globalSplitContainer.Panel1.Controls.Add(this.treeViewSplitContainer);
      // 
      // globalSplitContainer.Panel2
      // 
      this.globalSplitContainer.Panel2.Controls.Add(this.mruSplitContainer);
      this.globalSplitContainer.Size = new System.Drawing.Size(150, 500);
      this.globalSplitContainer.SplitterDistance = 385;
      this.globalSplitContainer.SplitterWidth = 2;
      this.globalSplitContainer.TabIndex = 4;
      // 
      // treeViewSplitContainer
      // 
      this.treeViewSplitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
      this.treeViewSplitContainer.Location = new System.Drawing.Point(0, 0);
      this.treeViewSplitContainer.Name = "treeViewSplitContainer";
      this.treeViewSplitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // treeViewSplitContainer.Panel1
      // 
      this.treeViewSplitContainer.Panel1.Controls.Add(this.modelTreeView);
      // 
      // treeViewSplitContainer.Panel2
      // 
      this.treeViewSplitContainer.Panel2.Controls.Add(this.graphicTreeView);
      this.treeViewSplitContainer.Size = new System.Drawing.Size(150, 385);
      this.treeViewSplitContainer.SplitterDistance = 184;
      this.treeViewSplitContainer.TabIndex = 3;
      // 
      // graphicTreeView
      // 
      this.graphicTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.graphicTreeView.Enabled = false;
      this.graphicTreeView.Location = new System.Drawing.Point(0, 0);
      this.graphicTreeView.Name = "graphicTreeView";
      this.graphicTreeView.Size = new System.Drawing.Size(150, 197);
      this.graphicTreeView.TabIndex = 0;
      this.graphicTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.graphicTreeView_AfterSelect);
      this.graphicTreeView.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.graphicTreeView_BeforeSelect);
      // 
      // mruSplitContainer
      // 
      this.mruSplitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
      this.mruSplitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
      this.mruSplitContainer.Location = new System.Drawing.Point(0, 0);
      this.mruSplitContainer.Name = "mruSplitContainer";
      this.mruSplitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // mruSplitContainer.Panel1
      // 
      this.mruSplitContainer.Panel1.Controls.Add(this.currentPanel);
      this.mruSplitContainer.Panel1MinSize = 20;
      // 
      // mruSplitContainer.Panel2
      // 
      this.mruSplitContainer.Panel2.Controls.Add(this.mruListBox);
      this.mruSplitContainer.Size = new System.Drawing.Size(150, 113);
      this.mruSplitContainer.SplitterDistance = 22;
      this.mruSplitContainer.TabIndex = 0;
      // 
      // mruListBox
      // 
      this.mruListBox.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.itemColumnHeader});
      this.mruListBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.mruListBox.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
      this.mruListBox.Location = new System.Drawing.Point(0, 0);
      this.mruListBox.Name = "mruListBox";
      this.mruListBox.Size = new System.Drawing.Size(150, 87);
      this.mruListBox.TabIndex = 0;
      this.mruListBox.UseCompatibleStateImageBehavior = false;
      this.mruListBox.View = System.Windows.Forms.View.Details;
      this.mruListBox.Resize += new System.EventHandler(this.mruListBox_Resize);
      // 
      // itemColumnHeader
      // 
      this.itemColumnHeader.Text = "";
      this.itemColumnHeader.Width = 146;
      // 
      // StencilChooser
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.globalSplitContainer);
      this.Name = "StencilChooser";
      this.Size = new System.Drawing.Size(150, 500);
      this.currentPanel.ResumeLayout(false);
      this.currentPanel.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.currentGraphiclPictureBox)).EndInit();
      ((System.ComponentModel.ISupportInitialize)(this.currentModelPictureBox)).EndInit();
      this.globalSplitContainer.Panel1.ResumeLayout(false);
      this.globalSplitContainer.Panel2.ResumeLayout(false);
      this.globalSplitContainer.ResumeLayout(false);
      this.treeViewSplitContainer.Panel1.ResumeLayout(false);
      this.treeViewSplitContainer.Panel2.ResumeLayout(false);
      this.treeViewSplitContainer.ResumeLayout(false);
      this.mruSplitContainer.Panel1.ResumeLayout(false);
      this.mruSplitContainer.Panel1.PerformLayout();
      this.mruSplitContainer.Panel2.ResumeLayout(false);
      this.mruSplitContainer.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    public System.Windows.Forms.TreeView modelTreeView;
    private System.Windows.Forms.Panel currentPanel;
    private System.Windows.Forms.PictureBox currentModelPictureBox;
    private System.Windows.Forms.PictureBox currentGraphiclPictureBox;
    private System.Windows.Forms.SplitContainer globalSplitContainer;
    private System.Windows.Forms.ListView mruListBox;
    private System.Windows.Forms.Label currentLabel;
    private System.Windows.Forms.ColumnHeader itemColumnHeader;
    private System.Windows.Forms.TreeView graphicTreeView;
    private System.Windows.Forms.SplitContainer treeViewSplitContainer;
    private System.Windows.Forms.SplitContainer mruSplitContainer;
  }
}
