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
      this.logView = new SysCAD.Log.LogView(5);
      this.SuspendLayout();
      // 
      // logView1
      // 
      this.logView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.logView.Location = new System.Drawing.Point(0, 0);
      this.logView.Name = "logView1";
      this.logView.Size = new System.Drawing.Size(703, 20);
      this.logView.TabIndex = 0;
      // 
      // ServiceTemporaryWindow
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(703, 272);
      this.Controls.Add(this.logView);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
      this.MaximizeBox = false;
      this.Name = "ServiceTemporaryWindow";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
      this.Text = "Service -- Temporary Window";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private SysCAD.Log.LogView logView;
  }
}

