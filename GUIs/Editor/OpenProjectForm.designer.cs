namespace SysCAD.Editor
{
  partial class OpenProjectForm
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
      this.repositoryURLTextBox = new System.Windows.Forms.TextBox();
      this.label1 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.projectListBox = new System.Windows.Forms.ListBox();
      this.label3 = new System.Windows.Forms.Label();
      this.openButton = new System.Windows.Forms.Button();
      this.cancelButton = new System.Windows.Forms.Button();
      this.listProjectsButton = new System.Windows.Forms.Button();
      this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
      this.SuspendLayout();
      // 
      // repositoryURLTextBox
      // 
      this.repositoryURLTextBox.Location = new System.Drawing.Point(98, 16);
      this.repositoryURLTextBox.Name = "repositoryURLTextBox";
      this.repositoryURLTextBox.Size = new System.Drawing.Size(274, 22);
      this.repositoryURLTextBox.TabIndex = 0;
      this.repositoryURLTextBox.Text = "ipc://SysCAD.Service/";
      this.repositoryURLTextBox.WordWrap = false;
      this.repositoryURLTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.repositoryURLTextBox_KeyPress);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Enabled = false;
      this.label1.Location = new System.Drawing.Point(12, 19);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(80, 17);
      this.label1.TabIndex = 1;
      this.label1.Text = "Repository:";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(95, 41);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(128, 17);
      this.label2.TabIndex = 2;
      this.label2.Text = "e.g. localhost:8065";
      // 
      // projectListBox
      // 
      this.projectListBox.Enabled = false;
      this.projectListBox.FormattingEnabled = true;
      this.projectListBox.ItemHeight = 16;
      this.projectListBox.Location = new System.Drawing.Point(15, 112);
      this.projectListBox.Name = "projectListBox";
      this.projectListBox.Size = new System.Drawing.Size(357, 180);
      this.projectListBox.Sorted = true;
      this.projectListBox.TabIndex = 3;
      this.projectListBox.DoubleClick += new System.EventHandler(this.projectListBox_DoubleClick);
      this.projectListBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.projectListBox_KeyPress);
      this.projectListBox.SelectedValueChanged += new System.EventHandler(this.projectListBox_SelectedValueChanged);
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(12, 92);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(59, 17);
      this.label3.TabIndex = 4;
      this.label3.Text = "Projects";
      // 
      // openButton
      // 
      this.openButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.openButton.Enabled = false;
      this.openButton.Location = new System.Drawing.Point(216, 304);
      this.openButton.Name = "openButton";
      this.openButton.Size = new System.Drawing.Size(75, 23);
      this.openButton.TabIndex = 5;
      this.openButton.Text = "Open";
      this.openButton.UseVisualStyleBackColor = true;
      // 
      // cancelButton
      // 
      this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.cancelButton.Location = new System.Drawing.Point(297, 304);
      this.cancelButton.Name = "cancelButton";
      this.cancelButton.Size = new System.Drawing.Size(75, 23);
      this.cancelButton.TabIndex = 6;
      this.cancelButton.Text = "Cancel";
      this.cancelButton.UseVisualStyleBackColor = true;
      // 
      // listProjectsButton
      // 
      this.listProjectsButton.Location = new System.Drawing.Point(12, 66);
      this.listProjectsButton.Name = "listProjectsButton";
      this.listProjectsButton.Size = new System.Drawing.Size(97, 23);
      this.listProjectsButton.TabIndex = 7;
      this.listProjectsButton.Text = "List Projects";
      this.listProjectsButton.UseVisualStyleBackColor = true;
      this.listProjectsButton.Click += new System.EventHandler(this.listProjectsButton_Click);
      // 
      // errorProvider1
      // 
      this.errorProvider1.ContainerControl = this;
      // 
      // OpenProjectForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(387, 341);
      this.Controls.Add(this.listProjectsButton);
      this.Controls.Add(this.cancelButton);
      this.Controls.Add(this.openButton);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.projectListBox);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.repositoryURLTextBox);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "OpenProjectForm";
      this.ShowIcon = false;
      this.ShowInTaskbar = false;
      this.Text = "OpenProjectForm";
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.TextBox repositoryURLTextBox;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.ListBox projectListBox;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Button openButton;
    private System.Windows.Forms.Button cancelButton;
    private System.Windows.Forms.Button listProjectsButton;
    private System.Windows.Forms.ErrorProvider errorProvider1;
  }
}