namespace SysCAD.Editor
{
  partial class GraphicStencilChangeForm
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
      this.graphicTreeView = new System.Windows.Forms.TreeView();
      this.okButton = new System.Windows.Forms.Button();
      this.cancelButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // graphicTreeView
      // 
      this.graphicTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.graphicTreeView.Location = new System.Drawing.Point(0, 0);
      this.graphicTreeView.Name = "graphicTreeView";
      this.graphicTreeView.Size = new System.Drawing.Size(292, 529);
      this.graphicTreeView.TabIndex = 0;
      this.graphicTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.graphicTreeView_AfterSelect);
      this.graphicTreeView.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.graphicTreeView_BeforeSelect);
      // 
      // okButton
      // 
      this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.okButton.Enabled = false;
      this.okButton.Location = new System.Drawing.Point(5, 535);
      this.okButton.Name = "okButton";
      this.okButton.Size = new System.Drawing.Size(75, 23);
      this.okButton.TabIndex = 1;
      this.okButton.Text = "OK";
      this.okButton.UseVisualStyleBackColor = true;
      // 
      // cancelButton
      // 
      this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.cancelButton.Location = new System.Drawing.Point(90, 535);
      this.cancelButton.Name = "cancelButton";
      this.cancelButton.Size = new System.Drawing.Size(75, 23);
      this.cancelButton.TabIndex = 2;
      this.cancelButton.Text = "Cancel";
      this.cancelButton.UseVisualStyleBackColor = true;
      // 
      // GraphicStencilChangeForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(292, 564);
      this.Controls.Add(this.cancelButton);
      this.Controls.Add(this.okButton);
      this.Controls.Add(this.graphicTreeView);
      this.Name = "GraphicStencilChangeForm";
      this.Text = "GraphicStencilChangeForm";
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TreeView graphicTreeView;
    private System.Windows.Forms.Button okButton;
    private System.Windows.Forms.Button cancelButton;
  }
}