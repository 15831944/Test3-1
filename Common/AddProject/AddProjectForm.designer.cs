namespace SysCAD
{
  partial class AddProjectForm
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
      this.addButton = new System.Windows.Forms.Button();
      this.cancelButton = new System.Windows.Forms.Button();
      this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
      this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
      this.nameLabel = new System.Windows.Forms.Label();
      this.nameTextBox = new System.Windows.Forms.TextBox();
      this.browseButton = new System.Windows.Forms.Button();
      this.pathTextBox = new System.Windows.Forms.TextBox();
      this.pathLabel = new System.Windows.Forms.Label();
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
      this.SuspendLayout();
      // 
      // addButton
      // 
      this.addButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.addButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.addButton.Enabled = false;
      this.addButton.Location = new System.Drawing.Point(309, 108);
      this.addButton.Margin = new System.Windows.Forms.Padding(2);
      this.addButton.Name = "addButton";
      this.addButton.Size = new System.Drawing.Size(56, 19);
      this.addButton.TabIndex = 5;
      this.addButton.Text = "Add";
      this.addButton.UseVisualStyleBackColor = true;
      // 
      // cancelButton
      // 
      this.cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.cancelButton.Location = new System.Drawing.Point(370, 108);
      this.cancelButton.Margin = new System.Windows.Forms.Padding(2);
      this.cancelButton.Name = "cancelButton";
      this.cancelButton.Size = new System.Drawing.Size(56, 19);
      this.cancelButton.TabIndex = 6;
      this.cancelButton.Text = "Cancel";
      this.cancelButton.UseVisualStyleBackColor = true;
      // 
      // errorProvider1
      // 
      this.errorProvider1.ContainerControl = this;
      // 
      // nameLabel
      // 
      this.nameLabel.AutoSize = true;
      this.nameLabel.Location = new System.Drawing.Point(12, 51);
      this.nameLabel.Name = "nameLabel";
      this.nameLabel.Size = new System.Drawing.Size(38, 13);
      this.nameLabel.TabIndex = 1;
      this.nameLabel.Text = "Name:";
      // 
      // nameTextBox
      // 
      this.nameTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.nameTextBox.Location = new System.Drawing.Point(55, 48);
      this.nameTextBox.Name = "nameTextBox";
      this.nameTextBox.Size = new System.Drawing.Size(371, 20);
      this.nameTextBox.TabIndex = 2;
      this.nameTextBox.TextChanged += new System.EventHandler(this.TextBox_TextChanged);
      // 
      // browseButton
      // 
      this.browseButton.Location = new System.Drawing.Point(12, 12);
      this.browseButton.Name = "browseButton";
      this.browseButton.Size = new System.Drawing.Size(75, 23);
      this.browseButton.TabIndex = 0;
      this.browseButton.Text = "Browse...";
      this.browseButton.UseVisualStyleBackColor = true;
      this.browseButton.Click += new System.EventHandler(this.browseButton_Click);
      // 
      // pathTextBox
      // 
      this.pathTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.pathTextBox.Location = new System.Drawing.Point(55, 74);
      this.pathTextBox.Name = "pathTextBox";
      this.pathTextBox.Size = new System.Drawing.Size(371, 20);
      this.pathTextBox.TabIndex = 4;
      this.pathTextBox.TextChanged += new System.EventHandler(this.TextBox_TextChanged);
      // 
      // pathLabel
      // 
      this.pathLabel.AutoSize = true;
      this.pathLabel.Location = new System.Drawing.Point(12, 77);
      this.pathLabel.Name = "pathLabel";
      this.pathLabel.Size = new System.Drawing.Size(32, 13);
      this.pathLabel.TabIndex = 3;
      this.pathLabel.Text = "Path:";
      // 
      // AddProjectForm
      // 
      this.AcceptButton = this.addButton;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.CancelButton = this.cancelButton;
      this.ClientSize = new System.Drawing.Size(437, 138);
      this.Controls.Add(this.pathTextBox);
      this.Controls.Add(this.pathLabel);
      this.Controls.Add(this.browseButton);
      this.Controls.Add(this.nameTextBox);
      this.Controls.Add(this.nameLabel);
      this.Controls.Add(this.cancelButton);
      this.Controls.Add(this.addButton);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Margin = new System.Windows.Forms.Padding(2);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.MinimumSize = new System.Drawing.Size(300, 170);
      this.Name = "AddProjectForm";
      this.ShowIcon = false;
      this.ShowInTaskbar = false;
      this.Text = "AddProjectForm";
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button addButton;
    private System.Windows.Forms.Button cancelButton;
    private System.Windows.Forms.ErrorProvider errorProvider1;
    private System.Windows.Forms.TextBox nameTextBox;
    private System.Windows.Forms.Label nameLabel;
    private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
    private System.Windows.Forms.Button browseButton;
    private System.Windows.Forms.TextBox pathTextBox;
    private System.Windows.Forms.Label pathLabel;
  }
}