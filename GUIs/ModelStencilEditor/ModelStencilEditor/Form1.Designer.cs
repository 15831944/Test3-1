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
      this.flowChart1.Size = new System.Drawing.Size(666, 650);
      this.flowChart1.TabIndex = 0;
      this.flowChart1.ArrowModified += new MindFusion.FlowChartX.ArrowMouseEvent(this.flowChart1_ArrowModified);
      this.flowChart1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.flowChart1_MouseClick);
      this.flowChart1.ArrowModifying += new MindFusion.FlowChartX.ArrowMouseEvent(this.flowChart1_ArrowModifying);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(666, 650);
      this.Controls.Add(this.flowChart1);
      this.Name = "Form1";
      this.Text = "Form1";
      this.ResumeLayout(false);

    }

    #endregion

    private MindFusion.FlowChartX.FlowChart flowChart1;



  }
}

