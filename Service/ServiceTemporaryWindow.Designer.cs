using MindFusion.FlowChartX;

namespace SysCAD.Service
{
  partial class ServiceTemporaryWindow
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    //protected override void Dispose(bool disposing)
    //{
    //  if (disposing && (components != null))
    //  {
    //    components.Dispose();
    //  }
    //  base.Dispose(disposing);
    //}

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ServiceTemporaryWindow));
      this.logView = new SysCAD.Log.LogView();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.splitContainer2 = new System.Windows.Forms.SplitContainer();
      this.flowChart = new MindFusion.FlowChartX.FlowChart();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.projectListView = new System.Windows.Forms.ListView();
      this.nameColumnHeader = new System.Windows.Forms.ColumnHeader();
      this.pathColumnHeader = new System.Windows.Forms.ColumnHeader();
      this.addProjectButton = new System.Windows.Forms.Button();
      this.removeProjectButton = new System.Windows.Forms.Button();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      this.splitContainer2.Panel1.SuspendLayout();
      this.splitContainer2.Panel2.SuspendLayout();
      this.splitContainer2.SuspendLayout();
      this.tableLayoutPanel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // logView
      // 
      this.logView.Active = true;
      this.logView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.logView.FullRowSelect = true;
      this.logView.GridLines = true;
      this.logView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
      this.logView.Location = new System.Drawing.Point(0, 0);
      this.logView.LogFile = "c:\\ServiceLog.txt";
      this.logView.Name = "logView";
      this.logView.Size = new System.Drawing.Size(876, 116);
      this.logView.TabIndex = 4;
      this.logView.TabStop = false;
      this.logView.UseCompatibleStateImageBehavior = false;
      this.logView.View = System.Windows.Forms.View.Details;
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 0);
      this.splitContainer1.Name = "splitContainer1";
      this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.logView);
      this.splitContainer1.Size = new System.Drawing.Size(876, 596);
      this.splitContainer1.SplitterDistance = 476;
      this.splitContainer1.TabIndex = 10;
      this.splitContainer1.TabStop = false;
      // 
      // splitContainer2
      // 
      this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer2.Location = new System.Drawing.Point(0, 0);
      this.splitContainer2.Name = "splitContainer2";
      // 
      // splitContainer2.Panel1
      // 
      this.splitContainer2.Panel1.Controls.Add(this.flowChart);
      // 
      // splitContainer2.Panel2
      // 
      this.splitContainer2.Panel2.Controls.Add(this.tableLayoutPanel1);
      this.splitContainer2.Size = new System.Drawing.Size(876, 476);
      this.splitContainer2.SplitterDistance = 637;
      this.splitContainer2.TabIndex = 10;
      this.splitContainer2.TabStop = false;
      // 
      // flowChart
      // 
      this.flowChart.CurrentCursor = System.Windows.Forms.Cursors.Default;
      this.flowChart.Cursor = System.Windows.Forms.Cursors.Default;
      this.flowChart.DefaultControlType = typeof(System.Windows.Forms.Button);
      this.flowChart.DocExtents = ((System.Drawing.RectangleF)(resources.GetObject("flowChart.DocExtents")));
      this.flowChart.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowChart.Enabled = false;
      this.flowChart.InplaceEditFont = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.flowChart.Location = new System.Drawing.Point(0, 0);
      this.flowChart.Name = "flowChart";
      this.flowChart.Size = new System.Drawing.Size(637, 476);
      this.flowChart.TabIndex = 5;
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 2;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel1.Controls.Add(this.projectListView, 0, 0);
      this.tableLayoutPanel1.Controls.Add(this.addProjectButton, 0, 1);
      this.tableLayoutPanel1.Controls.Add(this.removeProjectButton, 1, 1);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 2;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 89.70588F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 10.29412F));
      this.tableLayoutPanel1.Size = new System.Drawing.Size(235, 476);
      this.tableLayoutPanel1.TabIndex = 0;
      // 
      // projectListView
      // 
      this.projectListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.nameColumnHeader,
            this.pathColumnHeader});
      this.tableLayoutPanel1.SetColumnSpan(this.projectListView, 2);
      this.projectListView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.projectListView.FullRowSelect = true;
      this.projectListView.GridLines = true;
      this.projectListView.LabelEdit = true;
      this.projectListView.Location = new System.Drawing.Point(3, 3);
      this.projectListView.MultiSelect = false;
      this.projectListView.Name = "projectListView";
      this.projectListView.Size = new System.Drawing.Size(229, 420);
      this.projectListView.TabIndex = 2;
      this.projectListView.TabStop = false;
      this.projectListView.UseCompatibleStateImageBehavior = false;
      this.projectListView.View = System.Windows.Forms.View.Details;
      this.projectListView.SelectedIndexChanged += new System.EventHandler(this.projectListView_SelectedIndexChanged);
      // 
      // nameColumnHeader
      // 
      this.nameColumnHeader.Text = "Name";
      // 
      // pathColumnHeader
      // 
      this.pathColumnHeader.Text = "Path";
      this.pathColumnHeader.Width = 160;
      // 
      // addProjectButton
      // 
      this.addProjectButton.Location = new System.Drawing.Point(3, 429);
      this.addProjectButton.Name = "addProjectButton";
      this.addProjectButton.Size = new System.Drawing.Size(75, 23);
      this.addProjectButton.TabIndex = 0;
      this.addProjectButton.Text = "Add";
      this.addProjectButton.UseVisualStyleBackColor = true;
      this.addProjectButton.Click += new System.EventHandler(this.addProjectButton_Click);
      // 
      // removeProjectButton
      // 
      this.removeProjectButton.Enabled = false;
      this.removeProjectButton.Location = new System.Drawing.Point(120, 429);
      this.removeProjectButton.Name = "removeProjectButton";
      this.removeProjectButton.Size = new System.Drawing.Size(75, 23);
      this.removeProjectButton.TabIndex = 1;
      this.removeProjectButton.Text = "Remove Project";
      this.removeProjectButton.UseVisualStyleBackColor = true;
      this.removeProjectButton.Click += new System.EventHandler(this.removeProjectButton_Click);
      // 
      // ServiceTemporaryWindow
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(876, 596);
      this.Controls.Add(this.splitContainer1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.MaximizeBox = false;
      this.Name = "ServiceTemporaryWindow";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
      this.Text = "SysCAD.Service";
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel2.ResumeLayout(false);
      this.splitContainer1.ResumeLayout(false);
      this.splitContainer2.Panel1.ResumeLayout(false);
      this.splitContainer2.Panel2.ResumeLayout(false);
      this.splitContainer2.ResumeLayout(false);
      this.tableLayoutPanel1.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private SysCAD.Log.LogView logView;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private FlowChart flowChart;
    private System.Windows.Forms.SplitContainer splitContainer2;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private System.Windows.Forms.ListView projectListView;
    private System.Windows.Forms.Button addProjectButton;
    private System.Windows.Forms.Button removeProjectButton;
    private System.Windows.Forms.ColumnHeader nameColumnHeader;
    private System.Windows.Forms.ColumnHeader pathColumnHeader;
  }
}


