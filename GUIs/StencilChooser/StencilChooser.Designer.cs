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
      this.stencilChooserTreeView = new System.Windows.Forms.TreeView();
      this.SuspendLayout();
      // 
      // stencilChooserTreeView
      // 
      this.stencilChooserTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
      this.stencilChooserTreeView.Location = new System.Drawing.Point(0, 0);
      this.stencilChooserTreeView.Name = "stencilChooserTreeView";
      this.stencilChooserTreeView.Size = new System.Drawing.Size(150, 245);
      this.stencilChooserTreeView.TabIndex = 3;
      this.stencilChooserTreeView.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.stencilChooserTreeView_ItemDrag);
      // 
      // StencilChooser
      // 
      this.AllowDrop = true;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.Controls.Add(this.stencilChooserTreeView);
      this.Name = "StencilChooser";
      this.Size = new System.Drawing.Size(150, 245);
      this.ResumeLayout(false);

    }

    void stencilChooserTreeView_DragOver(object sender, System.Windows.Forms.DragEventArgs e)
    {
      throw new System.Exception("The method or operation is not implemented.");
    }

    void stencilChooserTreeView_DragLeave(object sender, System.EventArgs e)
    {
      throw new System.Exception("The method or operation is not implemented.");
    }

    void stencilChooserTreeView_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
    {
      throw new System.Exception("The method or operation is not implemented.");
    }

    void stencilChooserTreeView_DragDrop(object sender, System.Windows.Forms.DragEventArgs e)
    {
      throw new System.Exception("The method or operation is not implemented.");
    }

    #endregion

    public System.Windows.Forms.TreeView stencilChooserTreeView;

  }
}
