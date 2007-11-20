namespace SysCAD
{
  partial class AttachProjectForm
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
      this.attachButton = new System.Windows.Forms.Button();
      this.cancelButton = new System.Windows.Forms.Button();
      this.listProjectsButton = new System.Windows.Forms.Button();
      this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
      this.addButton = new System.Windows.Forms.Button();
      ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
      this.SuspendLayout();
      // 
      // repositoryURLTextBox
      // 
      this.repositoryURLTextBox.Location = new System.Drawing.Point(74, 13);
      this.repositoryURLTextBox.Margin = new System.Windows.Forms.Padding(2);
      this.repositoryURLTextBox.Name = "repositoryURLTextBox";
      this.repositoryURLTextBox.Size = new System.Drawing.Size(206, 20);
      this.repositoryURLTextBox.TabIndex = 3;
      this.repositoryURLTextBox.Text = "ipc://SysCAD.Service/";
      this.repositoryURLTextBox.WordWrap = false;
      this.repositoryURLTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.repositoryURLTextBox_KeyPress);
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Enabled = false;
      this.label1.Location = new System.Drawing.Point(9, 15);
      this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(60, 13);
      this.label1.TabIndex = 1;
      this.label1.Text = "Repository:";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(71, 33);
      this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(97, 13);
      this.label2.TabIndex = 2;
      this.label2.Text = "e.g. localhost:8065";
      // 
      // projectListBox
      // 
      this.projectListBox.Enabled = false;
      this.projectListBox.FormattingEnabled = true;
      this.projectListBox.Location = new System.Drawing.Point(11, 91);
      this.projectListBox.Margin = new System.Windows.Forms.Padding(2);
      this.projectListBox.Name = "projectListBox";
      this.projectListBox.Size = new System.Drawing.Size(269, 147);
      this.projectListBox.Sorted = true;
      this.projectListBox.TabIndex = 0;
      this.projectListBox.DoubleClick += new System.EventHandler(this.projectListBox_DoubleClick);
      this.projectListBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.projectListBox_KeyPress);
      this.projectListBox.SelectedValueChanged += new System.EventHandler(this.projectListBox_SelectedValueChanged);
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(9, 75);
      this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(45, 13);
      this.label3.TabIndex = 4;
      this.label3.Text = "Projects";
      // 
      // attachButton
      // 
      this.attachButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.attachButton.Enabled = false;
      this.attachButton.Location = new System.Drawing.Point(162, 247);
      this.attachButton.Margin = new System.Windows.Forms.Padding(2);
      this.attachButton.Name = "attachButton";
      this.attachButton.Size = new System.Drawing.Size(56, 19);
      this.attachButton.TabIndex = 5;
      this.attachButton.Text = "Attach";
      this.attachButton.UseVisualStyleBackColor = true;
      // 
      // cancelButton
      // 
      this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.cancelButton.Location = new System.Drawing.Point(223, 247);
      this.cancelButton.Margin = new System.Windows.Forms.Padding(2);
      this.cancelButton.Name = "cancelButton";
      this.cancelButton.Size = new System.Drawing.Size(56, 19);
      this.cancelButton.TabIndex = 6;
      this.cancelButton.Text = "Cancel";
      this.cancelButton.UseVisualStyleBackColor = true;
      // 
      // listProjectsButton
      // 
      this.listProjectsButton.Location = new System.Drawing.Point(9, 54);
      this.listProjectsButton.Margin = new System.Windows.Forms.Padding(2);
      this.listProjectsButton.Name = "listProjectsButton";
      this.listProjectsButton.Size = new System.Drawing.Size(73, 19);
      this.listProjectsButton.TabIndex = 7;
      this.listProjectsButton.Text = "List Projects";
      this.listProjectsButton.UseVisualStyleBackColor = true;
      this.listProjectsButton.Click += new System.EventHandler(this.listProjectsButton_Click);
      // 
      // errorProvider1
      // 
      this.errorProvider1.ContainerControl = this;
      // 
      // addButton
      // 
      this.addButton.Enabled = false;
      this.addButton.Location = new System.Drawing.Point(93, 54);
      this.addButton.Name = "addButton";
      this.addButton.Size = new System.Drawing.Size(75, 19);
      this.addButton.TabIndex = 8;
      this.addButton.Text = "Add...";
      this.addButton.UseVisualStyleBackColor = true;
      this.addButton.Click += new System.EventHandler(this.addButton_Click);
      // 
      // AttachProjectForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(290, 277);
      this.Controls.Add(this.addButton);
      this.Controls.Add(this.listProjectsButton);
      this.Controls.Add(this.cancelButton);
      this.Controls.Add(this.attachButton);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.projectListBox);
      this.Controls.Add(this.label2);
      this.Controls.Add(this.label1);
      this.Controls.Add(this.repositoryURLTextBox);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Margin = new System.Windows.Forms.Padding(2);
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "AttachProjectForm";
      this.ShowIcon = false;
      this.ShowInTaskbar = false;
      this.Text = "AttachProjectForm";
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
    private System.Windows.Forms.Button attachButton;
    private System.Windows.Forms.Button cancelButton;
    private System.Windows.Forms.Button listProjectsButton;
    private System.Windows.Forms.ErrorProvider errorProvider1;
    private System.Windows.Forms.Button addButton;
  }
}