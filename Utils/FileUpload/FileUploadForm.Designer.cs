namespace FileUpload
{
  partial class FileUploadForm
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FileUploadForm));
      this.projectsLabel = new System.Windows.Forms.Label();
      this.projectsListBox = new System.Windows.Forms.ListBox();
      this.categoriesListBox = new System.Windows.Forms.ListBox();
      this.categoriesLabel = new System.Windows.Forms.Label();
      this.notificationListTreeView = new TreeViewMS.TreeViewMS();
      this.NotificationListLabel = new System.Windows.Forms.Label();
      this.descriptionLabel = new System.Windows.Forms.Label();
      this.descriptionTextBox = new System.Windows.Forms.TextBox();
      this.uploadButton = new System.Windows.Forms.Button();
      this.nameTextBox = new System.Windows.Forms.TextBox();
      this.nameLabel = new System.Windows.Forms.Label();
      this.filenameTextBox = new System.Windows.Forms.TextBox();
      this.setFilenameButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // projectsLabel
      // 
      this.projectsLabel.AutoSize = true;
      this.projectsLabel.Location = new System.Drawing.Point(12, 9);
      this.projectsLabel.Name = "projectsLabel";
      this.projectsLabel.Size = new System.Drawing.Size(48, 13);
      this.projectsLabel.TabIndex = 0;
      this.projectsLabel.Text = "Projects:";
      // 
      // projectsListBox
      // 
      this.projectsListBox.FormattingEnabled = true;
      this.projectsListBox.Location = new System.Drawing.Point(135, 9);
      this.projectsListBox.Name = "projectsListBox";
      this.projectsListBox.Size = new System.Drawing.Size(292, 108);
      this.projectsListBox.TabIndex = 2;
      this.projectsListBox.SelectedIndexChanged += new System.EventHandler(this.projectsListBox_SelectedIndexChanged);
      this.projectsListBox.SelectedValueChanged += new System.EventHandler(this.projectsListBox_SelectedValueChanged);
      // 
      // categoriesListBox
      // 
      this.categoriesListBox.FormattingEnabled = true;
      this.categoriesListBox.Location = new System.Drawing.Point(135, 123);
      this.categoriesListBox.Name = "categoriesListBox";
      this.categoriesListBox.Size = new System.Drawing.Size(292, 108);
      this.categoriesListBox.TabIndex = 3;
      this.categoriesListBox.SelectedIndexChanged += new System.EventHandler(this.categoriesListBox_SelectedIndexChanged);
      // 
      // categoriesLabel
      // 
      this.categoriesLabel.AutoSize = true;
      this.categoriesLabel.Location = new System.Drawing.Point(12, 123);
      this.categoriesLabel.Name = "categoriesLabel";
      this.categoriesLabel.Size = new System.Drawing.Size(60, 13);
      this.categoriesLabel.TabIndex = 0;
      this.categoriesLabel.Text = "Categories:";
      // 
      // notificationListTreeView
      // 
      this.notificationListTreeView.Location = new System.Drawing.Point(135, 237);
      this.notificationListTreeView.Name = "notificationListTreeView";
      this.notificationListTreeView.SelectedNodes = ((System.Collections.ArrayList)(resources.GetObject("notificationListTreeView.SelectedNodes")));
      this.notificationListTreeView.Size = new System.Drawing.Size(292, 166);
      this.notificationListTreeView.TabIndex = 4;
      // 
      // NotificationListLabel
      // 
      this.NotificationListLabel.AutoSize = true;
      this.NotificationListLabel.Location = new System.Drawing.Point(12, 237);
      this.NotificationListLabel.Name = "NotificationListLabel";
      this.NotificationListLabel.Size = new System.Drawing.Size(82, 13);
      this.NotificationListLabel.TabIndex = 0;
      this.NotificationListLabel.Text = "Notification List:";
      // 
      // descriptionLabel
      // 
      this.descriptionLabel.AutoSize = true;
      this.descriptionLabel.Location = new System.Drawing.Point(12, 413);
      this.descriptionLabel.Name = "descriptionLabel";
      this.descriptionLabel.Size = new System.Drawing.Size(63, 13);
      this.descriptionLabel.TabIndex = 0;
      this.descriptionLabel.Text = "Description:";
      // 
      // descriptionTextBox
      // 
      this.descriptionTextBox.Location = new System.Drawing.Point(135, 409);
      this.descriptionTextBox.Name = "descriptionTextBox";
      this.descriptionTextBox.Size = new System.Drawing.Size(292, 20);
      this.descriptionTextBox.TabIndex = 5;
      this.descriptionTextBox.TextChanged += new System.EventHandler(this.descriptionTextBox_TextChanged);
      // 
      // uploadButton
      // 
      this.uploadButton.Enabled = false;
      this.uploadButton.Location = new System.Drawing.Point(352, 493);
      this.uploadButton.Name = "uploadButton";
      this.uploadButton.Size = new System.Drawing.Size(75, 23);
      this.uploadButton.TabIndex = 1;
      this.uploadButton.Text = "Upload";
      this.uploadButton.UseVisualStyleBackColor = true;
      this.uploadButton.Click += new System.EventHandler(this.uploadButton_Click);
      // 
      // nameTextBox
      // 
      this.nameTextBox.Location = new System.Drawing.Point(135, 435);
      this.nameTextBox.Name = "nameTextBox";
      this.nameTextBox.Size = new System.Drawing.Size(292, 20);
      this.nameTextBox.TabIndex = 6;
      // 
      // nameLabel
      // 
      this.nameLabel.AutoSize = true;
      this.nameLabel.Location = new System.Drawing.Point(12, 439);
      this.nameLabel.Name = "nameLabel";
      this.nameLabel.Size = new System.Drawing.Size(38, 13);
      this.nameLabel.TabIndex = 0;
      this.nameLabel.Text = "Name:";
      // 
      // filenameTextBox
      // 
      this.filenameTextBox.Location = new System.Drawing.Point(135, 461);
      this.filenameTextBox.Name = "filenameTextBox";
      this.filenameTextBox.ReadOnly = true;
      this.filenameTextBox.Size = new System.Drawing.Size(292, 20);
      this.filenameTextBox.TabIndex = 0;
      this.filenameTextBox.TabStop = false;
      // 
      // setFilenameButton
      // 
      this.setFilenameButton.Location = new System.Drawing.Point(12, 460);
      this.setFilenameButton.Name = "setFilenameButton";
      this.setFilenameButton.Size = new System.Drawing.Size(117, 23);
      this.setFilenameButton.TabIndex = 7;
      this.setFilenameButton.Text = "Set File";
      this.setFilenameButton.UseVisualStyleBackColor = true;
      this.setFilenameButton.Click += new System.EventHandler(this.setFilenameButton_Click);
      // 
      // FileUploadForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(439, 529);
      this.Controls.Add(this.setFilenameButton);
      this.Controls.Add(this.filenameTextBox);
      this.Controls.Add(this.nameTextBox);
      this.Controls.Add(this.nameLabel);
      this.Controls.Add(this.uploadButton);
      this.Controls.Add(this.descriptionTextBox);
      this.Controls.Add(this.descriptionLabel);
      this.Controls.Add(this.NotificationListLabel);
      this.Controls.Add(this.notificationListTreeView);
      this.Controls.Add(this.categoriesListBox);
      this.Controls.Add(this.categoriesLabel);
      this.Controls.Add(this.projectsListBox);
      this.Controls.Add(this.projectsLabel);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "FileUploadForm";
      this.Text = "Upload File";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Label projectsLabel;
    private System.Windows.Forms.ListBox projectsListBox;
    private System.Windows.Forms.ListBox categoriesListBox;
    private System.Windows.Forms.Label categoriesLabel;
    private TreeViewMS.TreeViewMS notificationListTreeView;
    private System.Windows.Forms.Label NotificationListLabel;
    private System.Windows.Forms.Label descriptionLabel;
    private System.Windows.Forms.TextBox descriptionTextBox;
    private System.Windows.Forms.Button uploadButton;
    private System.Windows.Forms.TextBox nameTextBox;
    private System.Windows.Forms.Label nameLabel;
    private System.Windows.Forms.TextBox filenameTextBox;
    private System.Windows.Forms.Button setFilenameButton;
  }
}

