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
      this.projectsListBox.Size = new System.Drawing.Size(292, 82);
      this.projectsListBox.TabIndex = 1;
      this.projectsListBox.SelectedValueChanged += new System.EventHandler(this.projectsListBox_SelectedValueChanged);
      // 
      // categoriesListBox
      // 
      this.categoriesListBox.FormattingEnabled = true;
      this.categoriesListBox.Location = new System.Drawing.Point(135, 97);
      this.categoriesListBox.Name = "categoriesListBox";
      this.categoriesListBox.Size = new System.Drawing.Size(292, 82);
      this.categoriesListBox.TabIndex = 3;
      this.categoriesListBox.SelectedValueChanged += new System.EventHandler(this.categoriesListBox_SelectedValueChanged);
      // 
      // categoriesLabel
      // 
      this.categoriesLabel.AutoSize = true;
      this.categoriesLabel.Location = new System.Drawing.Point(12, 97);
      this.categoriesLabel.Name = "categoriesLabel";
      this.categoriesLabel.Size = new System.Drawing.Size(60, 13);
      this.categoriesLabel.TabIndex = 2;
      this.categoriesLabel.Text = "Categories:";
      // 
      // notificationListTreeView
      // 
      this.notificationListTreeView.Location = new System.Drawing.Point(135, 185);
      this.notificationListTreeView.Name = "notificationListTreeView";
      this.notificationListTreeView.SelectedNodes = ((System.Collections.ArrayList)(resources.GetObject("notificationListTreeView.SelectedNodes")));
      this.notificationListTreeView.Size = new System.Drawing.Size(292, 148);
      this.notificationListTreeView.TabIndex = 4;
      // 
      // NotificationListLabel
      // 
      this.NotificationListLabel.AutoSize = true;
      this.NotificationListLabel.Location = new System.Drawing.Point(12, 185);
      this.NotificationListLabel.Name = "NotificationListLabel";
      this.NotificationListLabel.Size = new System.Drawing.Size(82, 13);
      this.NotificationListLabel.TabIndex = 5;
      this.NotificationListLabel.Text = "Notification List:";
      // 
      // descriptionLabel
      // 
      this.descriptionLabel.AutoSize = true;
      this.descriptionLabel.Location = new System.Drawing.Point(12, 339);
      this.descriptionLabel.Name = "descriptionLabel";
      this.descriptionLabel.Size = new System.Drawing.Size(63, 13);
      this.descriptionLabel.TabIndex = 6;
      this.descriptionLabel.Text = "Description:";
      // 
      // descriptionTextBox
      // 
      this.descriptionTextBox.Location = new System.Drawing.Point(135, 339);
      this.descriptionTextBox.Name = "descriptionTextBox";
      this.descriptionTextBox.Size = new System.Drawing.Size(292, 20);
      this.descriptionTextBox.TabIndex = 7;
      // 
      // uploadButton
      // 
      this.uploadButton.Location = new System.Drawing.Point(352, 365);
      this.uploadButton.Name = "uploadButton";
      this.uploadButton.Size = new System.Drawing.Size(75, 23);
      this.uploadButton.TabIndex = 8;
      this.uploadButton.Text = "Upload";
      this.uploadButton.UseVisualStyleBackColor = true;
      this.uploadButton.Click += new System.EventHandler(this.uploadButton_Click);
      // 
      // FileUploadForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(439, 400);
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
  }
}

