namespace Encrypt
{
    partial class EncryptProjectForm
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
          this.EncryptProjectButton = new System.Windows.Forms.Button();
          this.CreateBackupCheckBox = new System.Windows.Forms.CheckBox();
          this.EncryptPGMFilesCheckBox = new System.Windows.Forms.CheckBox();
          this.EncryptRCTFilesCheckBox = new System.Windows.Forms.CheckBox();
          this.EncryptDXFFilesCheckBox = new System.Windows.Forms.CheckBox();
          this.ProjectListBox = new System.Windows.Forms.ListBox();
          this.ProjectsLabel = new System.Windows.Forms.Label();
          this.EncryptGroupBox = new System.Windows.Forms.GroupBox();
          this.StatusStrip = new System.Windows.Forms.StatusStrip();
          this.ProgressToolStripProgressBar = new System.Windows.Forms.ToolStripProgressBar();
          this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
          this.SaveOptionsGroupBox = new System.Windows.Forms.GroupBox();
          this.AllowExcelReportsCheckBox = new System.Windows.Forms.CheckBox();
          this.AllowSaveCheckBox = new System.Windows.Forms.CheckBox();
          this.MinimumBuildNumericUpDown = new System.Windows.Forms.NumericUpDown();
          this.MinimumBuildLabel = new System.Windows.Forms.Label();
          this.BuildRangeGroupBox = new System.Windows.Forms.GroupBox();
          this.MaximumBuildLabel = new System.Windows.Forms.Label();
          this.MaximumBuildNumericUpDown = new System.Windows.Forms.NumericUpDown();
          this.BackupGroupBox = new System.Windows.Forms.GroupBox();
          this.VersionRangeGroupBox = new System.Windows.Forms.GroupBox();
          this.MaximumVersionLabel = new System.Windows.Forms.Label();
          this.MaximumVersionNumericUpDown = new System.Windows.Forms.NumericUpDown();
          this.MinimumVersionLabel = new System.Windows.Forms.Label();
          this.MinimumVersionNumericUpDown = new System.Windows.Forms.NumericUpDown();
          this.PasswordGroupBox = new System.Windows.Forms.GroupBox();
          this.PasswordTextBox = new System.Windows.Forms.TextBox();
          this.PasswordLabel = new System.Windows.Forms.Label();
          this.MACAddressGroupBox = new System.Windows.Forms.GroupBox();
          this.MACAddress01TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress02TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress03TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress04TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress05TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress10TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress09TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress08TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress07TextBox = new System.Windows.Forms.TextBox();
          this.MACAddress06TextBox = new System.Windows.Forms.TextBox();
          this.EncryptGroupBox.SuspendLayout();
          this.StatusStrip.SuspendLayout();
          this.SaveOptionsGroupBox.SuspendLayout();
          ((System.ComponentModel.ISupportInitialize)(this.MinimumBuildNumericUpDown)).BeginInit();
          this.BuildRangeGroupBox.SuspendLayout();
          ((System.ComponentModel.ISupportInitialize)(this.MaximumBuildNumericUpDown)).BeginInit();
          this.BackupGroupBox.SuspendLayout();
          this.VersionRangeGroupBox.SuspendLayout();
          ((System.ComponentModel.ISupportInitialize)(this.MaximumVersionNumericUpDown)).BeginInit();
          ((System.ComponentModel.ISupportInitialize)(this.MinimumVersionNumericUpDown)).BeginInit();
          this.PasswordGroupBox.SuspendLayout();
          this.MACAddressGroupBox.SuspendLayout();
          this.SuspendLayout();
          // 
          // EncryptProjectButton
          // 
          this.EncryptProjectButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.EncryptProjectButton.Location = new System.Drawing.Point(12, 462);
          this.EncryptProjectButton.Name = "EncryptProjectButton";
          this.EncryptProjectButton.Size = new System.Drawing.Size(500, 23);
          this.EncryptProjectButton.TabIndex = 1;
          this.EncryptProjectButton.Text = "Encrypt Project";
          this.EncryptProjectButton.UseVisualStyleBackColor = true;
          this.EncryptProjectButton.Click += new System.EventHandler(this.EncryptProjectButton_Click);
          // 
          // CreateBackupCheckBox
          // 
          this.CreateBackupCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.CreateBackupCheckBox.Checked = true;
          this.CreateBackupCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
          this.CreateBackupCheckBox.Location = new System.Drawing.Point(6, 19);
          this.CreateBackupCheckBox.Name = "CreateBackupCheckBox";
          this.CreateBackupCheckBox.Size = new System.Drawing.Size(97, 17);
          this.CreateBackupCheckBox.TabIndex = 2;
          this.CreateBackupCheckBox.Text = "Create Backup";
          this.CreateBackupCheckBox.UseVisualStyleBackColor = true;
          // 
          // EncryptPGMFilesCheckBox
          // 
          this.EncryptPGMFilesCheckBox.AutoSize = true;
          this.EncryptPGMFilesCheckBox.Location = new System.Drawing.Point(6, 19);
          this.EncryptPGMFilesCheckBox.Name = "EncryptPGMFilesCheckBox";
          this.EncryptPGMFilesCheckBox.Size = new System.Drawing.Size(74, 17);
          this.EncryptPGMFilesCheckBox.TabIndex = 3;
          this.EncryptPGMFilesCheckBox.Text = "PGM Files";
          this.EncryptPGMFilesCheckBox.UseVisualStyleBackColor = true;
          // 
          // EncryptRCTFilesCheckBox
          // 
          this.EncryptRCTFilesCheckBox.AutoSize = true;
          this.EncryptRCTFilesCheckBox.Location = new System.Drawing.Point(86, 19);
          this.EncryptRCTFilesCheckBox.Name = "EncryptRCTFilesCheckBox";
          this.EncryptRCTFilesCheckBox.Size = new System.Drawing.Size(72, 17);
          this.EncryptRCTFilesCheckBox.TabIndex = 4;
          this.EncryptRCTFilesCheckBox.Text = "RCT Files";
          this.EncryptRCTFilesCheckBox.UseVisualStyleBackColor = true;
          // 
          // EncryptDXFFilesCheckBox
          // 
          this.EncryptDXFFilesCheckBox.AutoSize = true;
          this.EncryptDXFFilesCheckBox.Location = new System.Drawing.Point(164, 19);
          this.EncryptDXFFilesCheckBox.Name = "EncryptDXFFilesCheckBox";
          this.EncryptDXFFilesCheckBox.Size = new System.Drawing.Size(71, 17);
          this.EncryptDXFFilesCheckBox.TabIndex = 5;
          this.EncryptDXFFilesCheckBox.Text = "DXF Files";
          this.EncryptDXFFilesCheckBox.UseVisualStyleBackColor = true;
          // 
          // ProjectListBox
          // 
          this.ProjectListBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.ProjectListBox.FormattingEnabled = true;
          this.ProjectListBox.HorizontalScrollbar = true;
          this.ProjectListBox.Location = new System.Drawing.Point(12, 348);
          this.ProjectListBox.Name = "ProjectListBox";
          this.ProjectListBox.Size = new System.Drawing.Size(500, 108);
          this.ProjectListBox.TabIndex = 6;
          this.ProjectListBox.DoubleClick += new System.EventHandler(this.ProjectListBox_DoubleClick);
          // 
          // ProjectsLabel
          // 
          this.ProjectsLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.ProjectsLabel.AutoSize = true;
          this.ProjectsLabel.Location = new System.Drawing.Point(12, 332);
          this.ProjectsLabel.Name = "ProjectsLabel";
          this.ProjectsLabel.Size = new System.Drawing.Size(48, 13);
          this.ProjectsLabel.TabIndex = 7;
          this.ProjectsLabel.Text = "Projects:";
          // 
          // EncryptGroupBox
          // 
          this.EncryptGroupBox.Controls.Add(this.EncryptPGMFilesCheckBox);
          this.EncryptGroupBox.Controls.Add(this.EncryptRCTFilesCheckBox);
          this.EncryptGroupBox.Controls.Add(this.EncryptDXFFilesCheckBox);
          this.EncryptGroupBox.Location = new System.Drawing.Point(265, 61);
          this.EncryptGroupBox.Name = "EncryptGroupBox";
          this.EncryptGroupBox.Size = new System.Drawing.Size(244, 43);
          this.EncryptGroupBox.TabIndex = 9;
          this.EncryptGroupBox.TabStop = false;
          this.EncryptGroupBox.Text = "Encrypt";
          // 
          // StatusStrip
          // 
          this.StatusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ProgressToolStripProgressBar,
            this.toolStripStatusLabel1});
          this.StatusStrip.Location = new System.Drawing.Point(0, 488);
          this.StatusStrip.Name = "StatusStrip";
          this.StatusStrip.Size = new System.Drawing.Size(524, 22);
          this.StatusStrip.SizingGrip = false;
          this.StatusStrip.TabIndex = 10;
          // 
          // ProgressToolStripProgressBar
          // 
          this.ProgressToolStripProgressBar.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
          this.ProgressToolStripProgressBar.Name = "ProgressToolStripProgressBar";
          this.ProgressToolStripProgressBar.Size = new System.Drawing.Size(520, 16);
          this.ProgressToolStripProgressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
          // 
          // toolStripStatusLabel1
          // 
          this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
          this.toolStripStatusLabel1.Size = new System.Drawing.Size(0, 17);
          // 
          // SaveOptionsGroupBox
          // 
          this.SaveOptionsGroupBox.Controls.Add(this.AllowExcelReportsCheckBox);
          this.SaveOptionsGroupBox.Controls.Add(this.AllowSaveCheckBox);
          this.SaveOptionsGroupBox.Location = new System.Drawing.Point(15, 61);
          this.SaveOptionsGroupBox.Name = "SaveOptionsGroupBox";
          this.SaveOptionsGroupBox.Size = new System.Drawing.Size(244, 43);
          this.SaveOptionsGroupBox.TabIndex = 11;
          this.SaveOptionsGroupBox.TabStop = false;
          this.SaveOptionsGroupBox.Text = "Save Options";
          // 
          // AllowExcelReportsCheckBox
          // 
          this.AllowExcelReportsCheckBox.AutoSize = true;
          this.AllowExcelReportsCheckBox.Location = new System.Drawing.Point(86, 19);
          this.AllowExcelReportsCheckBox.Name = "AllowExcelReportsCheckBox";
          this.AllowExcelReportsCheckBox.Size = new System.Drawing.Size(120, 17);
          this.AllowExcelReportsCheckBox.TabIndex = 0;
          this.AllowExcelReportsCheckBox.Text = "Allow Excel Reports";
          this.AllowExcelReportsCheckBox.UseVisualStyleBackColor = true;
          // 
          // AllowSaveCheckBox
          // 
          this.AllowSaveCheckBox.AutoSize = true;
          this.AllowSaveCheckBox.Location = new System.Drawing.Point(6, 19);
          this.AllowSaveCheckBox.Name = "AllowSaveCheckBox";
          this.AllowSaveCheckBox.Size = new System.Drawing.Size(79, 17);
          this.AllowSaveCheckBox.TabIndex = 0;
          this.AllowSaveCheckBox.Text = "Allow Save";
          this.AllowSaveCheckBox.UseVisualStyleBackColor = true;
          // 
          // MinimumBuildNumericUpDown
          // 
          this.MinimumBuildNumericUpDown.Location = new System.Drawing.Point(86, 23);
          this.MinimumBuildNumericUpDown.Maximum = new decimal(new int[] {
            32767,
            0,
            0,
            0});
          this.MinimumBuildNumericUpDown.Name = "MinimumBuildNumericUpDown";
          this.MinimumBuildNumericUpDown.Size = new System.Drawing.Size(152, 20);
          this.MinimumBuildNumericUpDown.TabIndex = 12;
          // 
          // MinimumBuildLabel
          // 
          this.MinimumBuildLabel.AutoSize = true;
          this.MinimumBuildLabel.Location = new System.Drawing.Point(6, 25);
          this.MinimumBuildLabel.Name = "MinimumBuildLabel";
          this.MinimumBuildLabel.Size = new System.Drawing.Size(74, 13);
          this.MinimumBuildLabel.TabIndex = 13;
          this.MinimumBuildLabel.Text = "Minimum Build";
          // 
          // BuildRangeGroupBox
          // 
          this.BuildRangeGroupBox.Controls.Add(this.MaximumBuildLabel);
          this.BuildRangeGroupBox.Controls.Add(this.MaximumBuildNumericUpDown);
          this.BuildRangeGroupBox.Controls.Add(this.MinimumBuildLabel);
          this.BuildRangeGroupBox.Controls.Add(this.MinimumBuildNumericUpDown);
          this.BuildRangeGroupBox.Location = new System.Drawing.Point(15, 110);
          this.BuildRangeGroupBox.Name = "BuildRangeGroupBox";
          this.BuildRangeGroupBox.Size = new System.Drawing.Size(244, 82);
          this.BuildRangeGroupBox.TabIndex = 14;
          this.BuildRangeGroupBox.TabStop = false;
          this.BuildRangeGroupBox.Text = "Allowed Build Range";
          // 
          // MaximumBuildLabel
          // 
          this.MaximumBuildLabel.AutoSize = true;
          this.MaximumBuildLabel.Location = new System.Drawing.Point(6, 51);
          this.MaximumBuildLabel.Name = "MaximumBuildLabel";
          this.MaximumBuildLabel.Size = new System.Drawing.Size(77, 13);
          this.MaximumBuildLabel.TabIndex = 15;
          this.MaximumBuildLabel.Text = "Maximum Build";
          // 
          // MaximumBuildNumericUpDown
          // 
          this.MaximumBuildNumericUpDown.Location = new System.Drawing.Point(86, 49);
          this.MaximumBuildNumericUpDown.Maximum = new decimal(new int[] {
            32767,
            0,
            0,
            0});
          this.MaximumBuildNumericUpDown.Name = "MaximumBuildNumericUpDown";
          this.MaximumBuildNumericUpDown.Size = new System.Drawing.Size(152, 20);
          this.MaximumBuildNumericUpDown.TabIndex = 14;
          // 
          // BackupGroupBox
          // 
          this.BackupGroupBox.Controls.Add(this.CreateBackupCheckBox);
          this.BackupGroupBox.Location = new System.Drawing.Point(15, 12);
          this.BackupGroupBox.Name = "BackupGroupBox";
          this.BackupGroupBox.Size = new System.Drawing.Size(108, 43);
          this.BackupGroupBox.TabIndex = 15;
          this.BackupGroupBox.TabStop = false;
          this.BackupGroupBox.Text = "Backup";
          // 
          // VersionRangeGroupBox
          // 
          this.VersionRangeGroupBox.Controls.Add(this.MaximumVersionLabel);
          this.VersionRangeGroupBox.Controls.Add(this.MaximumVersionNumericUpDown);
          this.VersionRangeGroupBox.Controls.Add(this.MinimumVersionLabel);
          this.VersionRangeGroupBox.Controls.Add(this.MinimumVersionNumericUpDown);
          this.VersionRangeGroupBox.Location = new System.Drawing.Point(265, 110);
          this.VersionRangeGroupBox.Name = "VersionRangeGroupBox";
          this.VersionRangeGroupBox.Size = new System.Drawing.Size(244, 82);
          this.VersionRangeGroupBox.TabIndex = 16;
          this.VersionRangeGroupBox.TabStop = false;
          this.VersionRangeGroupBox.Text = "Allowed Version Range";
          // 
          // MaximumVersionLabel
          // 
          this.MaximumVersionLabel.AutoSize = true;
          this.MaximumVersionLabel.Location = new System.Drawing.Point(6, 51);
          this.MaximumVersionLabel.Name = "MaximumVersionLabel";
          this.MaximumVersionLabel.Size = new System.Drawing.Size(89, 13);
          this.MaximumVersionLabel.TabIndex = 15;
          this.MaximumVersionLabel.Text = "Maximum Version";
          // 
          // MaximumVersionNumericUpDown
          // 
          this.MaximumVersionNumericUpDown.Location = new System.Drawing.Point(101, 49);
          this.MaximumVersionNumericUpDown.Maximum = new decimal(new int[] {
            32767,
            0,
            0,
            0});
          this.MaximumVersionNumericUpDown.Name = "MaximumVersionNumericUpDown";
          this.MaximumVersionNumericUpDown.Size = new System.Drawing.Size(132, 20);
          this.MaximumVersionNumericUpDown.TabIndex = 14;
          // 
          // MinimumVersionLabel
          // 
          this.MinimumVersionLabel.AutoSize = true;
          this.MinimumVersionLabel.Location = new System.Drawing.Point(6, 25);
          this.MinimumVersionLabel.Name = "MinimumVersionLabel";
          this.MinimumVersionLabel.Size = new System.Drawing.Size(86, 13);
          this.MinimumVersionLabel.TabIndex = 13;
          this.MinimumVersionLabel.Text = "Minimum Version";
          // 
          // MinimumVersionNumericUpDown
          // 
          this.MinimumVersionNumericUpDown.Location = new System.Drawing.Point(101, 23);
          this.MinimumVersionNumericUpDown.Maximum = new decimal(new int[] {
            32767,
            0,
            0,
            0});
          this.MinimumVersionNumericUpDown.Name = "MinimumVersionNumericUpDown";
          this.MinimumVersionNumericUpDown.Size = new System.Drawing.Size(132, 20);
          this.MinimumVersionNumericUpDown.TabIndex = 12;
          // 
          // PasswordGroupBox
          // 
          this.PasswordGroupBox.Controls.Add(this.PasswordTextBox);
          this.PasswordGroupBox.Controls.Add(this.PasswordLabel);
          this.PasswordGroupBox.Location = new System.Drawing.Point(15, 278);
          this.PasswordGroupBox.Name = "PasswordGroupBox";
          this.PasswordGroupBox.Size = new System.Drawing.Size(244, 44);
          this.PasswordGroupBox.TabIndex = 17;
          this.PasswordGroupBox.TabStop = false;
          this.PasswordGroupBox.Text = "Open Password (Leave blank for none)";
          // 
          // PasswordTextBox
          // 
          this.PasswordTextBox.Location = new System.Drawing.Point(65, 18);
          this.PasswordTextBox.Name = "PasswordTextBox";
          this.PasswordTextBox.Size = new System.Drawing.Size(167, 20);
          this.PasswordTextBox.TabIndex = 1;
          // 
          // PasswordLabel
          // 
          this.PasswordLabel.AutoSize = true;
          this.PasswordLabel.Location = new System.Drawing.Point(6, 21);
          this.PasswordLabel.Name = "PasswordLabel";
          this.PasswordLabel.Size = new System.Drawing.Size(53, 13);
          this.PasswordLabel.TabIndex = 0;
          this.PasswordLabel.Text = "Password";
          // 
          // MACAddressGroupBox
          // 
          this.MACAddressGroupBox.Controls.Add(this.MACAddress10TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress09TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress08TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress07TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress06TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress05TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress04TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress03TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress02TextBox);
          this.MACAddressGroupBox.Controls.Add(this.MACAddress01TextBox);
          this.MACAddressGroupBox.Location = new System.Drawing.Point(15, 198);
          this.MACAddressGroupBox.Name = "MACAddressGroupBox";
          this.MACAddressGroupBox.Size = new System.Drawing.Size(494, 74);
          this.MACAddressGroupBox.TabIndex = 18;
          this.MACAddressGroupBox.TabStop = false;
          this.MACAddressGroupBox.Text = "Allowed MAC Addresses (Leave all blank for allow all)";
          // 
          // MACAddress01TextBox
          // 
          this.MACAddress01TextBox.Location = new System.Drawing.Point(9, 19);
          this.MACAddress01TextBox.Name = "MACAddress01TextBox";
          this.MACAddress01TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress01TextBox.TabIndex = 0;
          // 
          // MACAddress02TextBox
          // 
          this.MACAddress02TextBox.Location = new System.Drawing.Point(105, 19);
          this.MACAddress02TextBox.Name = "MACAddress02TextBox";
          this.MACAddress02TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress02TextBox.TabIndex = 1;
          // 
          // MACAddress03TextBox
          // 
          this.MACAddress03TextBox.Location = new System.Drawing.Point(201, 19);
          this.MACAddress03TextBox.Name = "MACAddress03TextBox";
          this.MACAddress03TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress03TextBox.TabIndex = 2;
          // 
          // MACAddress04TextBox
          // 
          this.MACAddress04TextBox.Location = new System.Drawing.Point(297, 19);
          this.MACAddress04TextBox.Name = "MACAddress04TextBox";
          this.MACAddress04TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress04TextBox.TabIndex = 3;
          // 
          // MACAddress05TextBox
          // 
          this.MACAddress05TextBox.Location = new System.Drawing.Point(393, 19);
          this.MACAddress05TextBox.Name = "MACAddress05TextBox";
          this.MACAddress05TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress05TextBox.TabIndex = 4;
          // 
          // MACAddress10TextBox
          // 
          this.MACAddress10TextBox.Location = new System.Drawing.Point(393, 45);
          this.MACAddress10TextBox.Name = "MACAddress10TextBox";
          this.MACAddress10TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress10TextBox.TabIndex = 9;
          // 
          // MACAddress09TextBox
          // 
          this.MACAddress09TextBox.Location = new System.Drawing.Point(297, 45);
          this.MACAddress09TextBox.Name = "MACAddress09TextBox";
          this.MACAddress09TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress09TextBox.TabIndex = 8;
          // 
          // MACAddress08TextBox
          // 
          this.MACAddress08TextBox.Location = new System.Drawing.Point(201, 45);
          this.MACAddress08TextBox.Name = "MACAddress08TextBox";
          this.MACAddress08TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress08TextBox.TabIndex = 7;
          // 
          // MACAddress07TextBox
          // 
          this.MACAddress07TextBox.Location = new System.Drawing.Point(105, 45);
          this.MACAddress07TextBox.Name = "MACAddress07TextBox";
          this.MACAddress07TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress07TextBox.TabIndex = 6;
          // 
          // MACAddress06TextBox
          // 
          this.MACAddress06TextBox.Location = new System.Drawing.Point(9, 45);
          this.MACAddress06TextBox.Name = "MACAddress06TextBox";
          this.MACAddress06TextBox.Size = new System.Drawing.Size(90, 20);
          this.MACAddress06TextBox.TabIndex = 5;
          // 
          // EncryptProjectForm
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(524, 510);
          this.Controls.Add(this.MACAddressGroupBox);
          this.Controls.Add(this.PasswordGroupBox);
          this.Controls.Add(this.VersionRangeGroupBox);
          this.Controls.Add(this.BackupGroupBox);
          this.Controls.Add(this.BuildRangeGroupBox);
          this.Controls.Add(this.SaveOptionsGroupBox);
          this.Controls.Add(this.StatusStrip);
          this.Controls.Add(this.EncryptGroupBox);
          this.Controls.Add(this.ProjectsLabel);
          this.Controls.Add(this.ProjectListBox);
          this.Controls.Add(this.EncryptProjectButton);
          this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
          this.Name = "EncryptProjectForm";
          this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
          this.Text = "Encrypt Project";
          this.Load += new System.EventHandler(this.EncryptProjectForm_Load);
          this.EncryptGroupBox.ResumeLayout(false);
          this.EncryptGroupBox.PerformLayout();
          this.StatusStrip.ResumeLayout(false);
          this.StatusStrip.PerformLayout();
          this.SaveOptionsGroupBox.ResumeLayout(false);
          this.SaveOptionsGroupBox.PerformLayout();
          ((System.ComponentModel.ISupportInitialize)(this.MinimumBuildNumericUpDown)).EndInit();
          this.BuildRangeGroupBox.ResumeLayout(false);
          this.BuildRangeGroupBox.PerformLayout();
          ((System.ComponentModel.ISupportInitialize)(this.MaximumBuildNumericUpDown)).EndInit();
          this.BackupGroupBox.ResumeLayout(false);
          this.VersionRangeGroupBox.ResumeLayout(false);
          this.VersionRangeGroupBox.PerformLayout();
          ((System.ComponentModel.ISupportInitialize)(this.MaximumVersionNumericUpDown)).EndInit();
          ((System.ComponentModel.ISupportInitialize)(this.MinimumVersionNumericUpDown)).EndInit();
          this.PasswordGroupBox.ResumeLayout(false);
          this.PasswordGroupBox.PerformLayout();
          this.MACAddressGroupBox.ResumeLayout(false);
          this.MACAddressGroupBox.PerformLayout();
          this.ResumeLayout(false);
          this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button EncryptProjectButton;
      private System.Windows.Forms.CheckBox CreateBackupCheckBox;
      private System.Windows.Forms.CheckBox EncryptPGMFilesCheckBox;
      private System.Windows.Forms.CheckBox EncryptRCTFilesCheckBox;
      private System.Windows.Forms.CheckBox EncryptDXFFilesCheckBox;
      private System.Windows.Forms.ListBox ProjectListBox;
      private System.Windows.Forms.Label ProjectsLabel;
      private System.Windows.Forms.GroupBox EncryptGroupBox;
      private System.Windows.Forms.StatusStrip StatusStrip;
      private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
      private System.Windows.Forms.ToolStripProgressBar ProgressToolStripProgressBar;
      private System.Windows.Forms.GroupBox SaveOptionsGroupBox;
      private System.Windows.Forms.CheckBox AllowExcelReportsCheckBox;
      private System.Windows.Forms.CheckBox AllowSaveCheckBox;
      private System.Windows.Forms.NumericUpDown MinimumBuildNumericUpDown;
      private System.Windows.Forms.Label MinimumBuildLabel;
      private System.Windows.Forms.GroupBox BuildRangeGroupBox;
      private System.Windows.Forms.Label MaximumBuildLabel;
      private System.Windows.Forms.NumericUpDown MaximumBuildNumericUpDown;
      private System.Windows.Forms.GroupBox BackupGroupBox;
      private System.Windows.Forms.GroupBox VersionRangeGroupBox;
      private System.Windows.Forms.Label MaximumVersionLabel;
      private System.Windows.Forms.NumericUpDown MaximumVersionNumericUpDown;
      private System.Windows.Forms.Label MinimumVersionLabel;
      private System.Windows.Forms.NumericUpDown MinimumVersionNumericUpDown;
      private System.Windows.Forms.GroupBox PasswordGroupBox;
      private System.Windows.Forms.TextBox PasswordTextBox;
      private System.Windows.Forms.Label PasswordLabel;
      private System.Windows.Forms.GroupBox MACAddressGroupBox;
      private System.Windows.Forms.TextBox MACAddress01TextBox;
      private System.Windows.Forms.TextBox MACAddress05TextBox;
      private System.Windows.Forms.TextBox MACAddress04TextBox;
      private System.Windows.Forms.TextBox MACAddress03TextBox;
      private System.Windows.Forms.TextBox MACAddress02TextBox;
      private System.Windows.Forms.TextBox MACAddress10TextBox;
      private System.Windows.Forms.TextBox MACAddress09TextBox;
      private System.Windows.Forms.TextBox MACAddress08TextBox;
      private System.Windows.Forms.TextBox MACAddress07TextBox;
      private System.Windows.Forms.TextBox MACAddress06TextBox;
    }
}

