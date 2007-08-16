namespace TestApp
{
  partial class TestAppForm
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
      this.stencilChooser = new SysCAD.StencilChooser();
      this.SuspendLayout();
      // 
      // stencilListView
      // 
      this.stencilChooser.Dock = System.Windows.Forms.DockStyle.Fill;
      this.stencilChooser.Location = new System.Drawing.Point(0, 0);
      this.stencilChooser.Name = "stencilListView";
      this.stencilChooser.Size = new System.Drawing.Size(284, 264);
      this.stencilChooser.TabIndex = 0;
      // 
      // TestApp
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(284, 264);
      this.Controls.Add(this.stencilChooser);
      this.Name = "TestApp";
      this.Text = "TestApp";
      this.ResumeLayout(false);

    }

    #endregion

    private SysCAD.StencilChooser stencilChooser;
  }
}

