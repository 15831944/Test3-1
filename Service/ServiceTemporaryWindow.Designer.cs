using MindFusion.FlowChartX;

namespace Service
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
      this.components = new System.ComponentModel.Container();
      this.logView = new SysCAD.Log.LogView();
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.flowChart = new FlowChart();
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
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
      this.logView.Size = new System.Drawing.Size(876, 315);
      this.logView.TabIndex = 0;
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
      this.splitContainer1.Panel1.Controls.Add(this.flowChart);
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.logView);
      this.splitContainer1.Size = new System.Drawing.Size(876, 596);
      this.splitContainer1.SplitterDistance = 277;
      this.splitContainer1.TabIndex = 1;
      // 
      // button1
      // 
      this.flowChart.Dock = System.Windows.Forms.DockStyle.Fill;
      this.flowChart.Location = new System.Drawing.Point(0, 0);
      this.flowChart.Name = "flowChart";
      this.flowChart.Size = new System.Drawing.Size(876, 277);
      this.flowChart.TabIndex = 0;
      this.flowChart.Text = "flowChart";
      this.flowChart.Enabled = false;
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
      this.Text = "Service -- Temporary Window";
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel2.ResumeLayout(false);
      this.splitContainer1.ResumeLayout(false);
      this.ResumeLayout(false);

    }

    #endregion

    private SysCAD.Log.LogView logView;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private FlowChart flowChart;
  }
}


