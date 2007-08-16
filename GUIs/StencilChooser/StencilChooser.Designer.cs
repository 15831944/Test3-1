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
      this.listView = new System.Windows.Forms.ListView();
      this.SuspendLayout();
      // 
      // listView
      // 
      this.listView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.listView.Location = new System.Drawing.Point(0, 0);
      this.listView.Name = "listView";
      this.listView.Size = new System.Drawing.Size(458, 305);
      this.listView.TabIndex = 0;
      this.listView.UseCompatibleStateImageBehavior = false;
      // 
      // StencilChooser
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.listView);
      this.Name = "StencilChooser";
      this.Size = new System.Drawing.Size(458, 305);
      this.ResumeLayout(false);

    }

    #endregion

    public System.Windows.Forms.ListView listView;
  }
}
