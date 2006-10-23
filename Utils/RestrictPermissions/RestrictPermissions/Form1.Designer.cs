namespace RestrictPermissions
{
  partial class Form1
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
      this.setButton = new System.Windows.Forms.Button();
      this.ReadOnlyCheckBox = new System.Windows.Forms.CheckBox();
      this.checkBox1 = new System.Windows.Forms.CheckBox();
      this.checkBox2 = new System.Windows.Forms.CheckBox();
      this.checkBox3 = new System.Windows.Forms.CheckBox();
      this.checkBox4 = new System.Windows.Forms.CheckBox();
      this.checkBox5 = new System.Windows.Forms.CheckBox();
      this.checkBox6 = new System.Windows.Forms.CheckBox();
      this.checkBox7 = new System.Windows.Forms.CheckBox();
      this.loadButton = new System.Windows.Forms.Button();
      this.encryptButton = new System.Windows.Forms.Button();
      this.decryptButton = new System.Windows.Forms.Button();
      this.passwordSetButton = new System.Windows.Forms.Button();
      this.SuspendLayout();
      // 
      // setButton
      // 
      this.setButton.Enabled = false;
      this.setButton.Location = new System.Drawing.Point(97, 163);
      this.setButton.Name = "setButton";
      this.setButton.Size = new System.Drawing.Size(75, 23);
      this.setButton.TabIndex = 0;
      this.setButton.Text = "Set";
      this.setButton.UseVisualStyleBackColor = true;
      this.setButton.Click += new System.EventHandler(this.setButton_Click);
      // 
      // ReadOnlyCheckBox
      // 
      this.ReadOnlyCheckBox.AutoSize = true;
      this.ReadOnlyCheckBox.Enabled = false;
      this.ReadOnlyCheckBox.Location = new System.Drawing.Point(13, 13);
      this.ReadOnlyCheckBox.Name = "ReadOnlyCheckBox";
      this.ReadOnlyCheckBox.Size = new System.Drawing.Size(73, 17);
      this.ReadOnlyCheckBox.TabIndex = 3;
      this.ReadOnlyCheckBox.Text = "ReadOnly";
      this.ReadOnlyCheckBox.UseVisualStyleBackColor = true;
      this.ReadOnlyCheckBox.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // checkBox1
      // 
      this.checkBox1.AutoSize = true;
      this.checkBox1.Enabled = false;
      this.checkBox1.Location = new System.Drawing.Point(13, 36);
      this.checkBox1.Name = "checkBox1";
      this.checkBox1.Size = new System.Drawing.Size(53, 17);
      this.checkBox1.TabIndex = 4;
      this.checkBox1.Text = "Alpha";
      this.checkBox1.UseVisualStyleBackColor = true;
      this.checkBox1.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // checkBox2
      // 
      this.checkBox2.AutoSize = true;
      this.checkBox2.Enabled = false;
      this.checkBox2.Location = new System.Drawing.Point(13, 59);
      this.checkBox2.Name = "checkBox2";
      this.checkBox2.Size = new System.Drawing.Size(48, 17);
      this.checkBox2.TabIndex = 5;
      this.checkBox2.Text = "Beta";
      this.checkBox2.UseVisualStyleBackColor = true;
      this.checkBox2.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // checkBox3
      // 
      this.checkBox3.AutoSize = true;
      this.checkBox3.Enabled = false;
      this.checkBox3.Location = new System.Drawing.Point(13, 82);
      this.checkBox3.Name = "checkBox3";
      this.checkBox3.Size = new System.Drawing.Size(62, 17);
      this.checkBox3.TabIndex = 6;
      this.checkBox3.Text = "Gamma";
      this.checkBox3.UseVisualStyleBackColor = true;
      this.checkBox3.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // checkBox4
      // 
      this.checkBox4.AutoSize = true;
      this.checkBox4.Enabled = false;
      this.checkBox4.Location = new System.Drawing.Point(112, 36);
      this.checkBox4.Name = "checkBox4";
      this.checkBox4.Size = new System.Drawing.Size(60, 17);
      this.checkBox4.TabIndex = 8;
      this.checkBox4.Text = "Epsilon";
      this.checkBox4.UseVisualStyleBackColor = true;
      this.checkBox4.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // checkBox5
      // 
      this.checkBox5.AutoSize = true;
      this.checkBox5.Enabled = false;
      this.checkBox5.Location = new System.Drawing.Point(112, 13);
      this.checkBox5.Name = "checkBox5";
      this.checkBox5.Size = new System.Drawing.Size(51, 17);
      this.checkBox5.TabIndex = 7;
      this.checkBox5.Text = "Delta";
      this.checkBox5.UseVisualStyleBackColor = true;
      this.checkBox5.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // checkBox6
      // 
      this.checkBox6.AutoSize = true;
      this.checkBox6.Enabled = false;
      this.checkBox6.Location = new System.Drawing.Point(112, 59);
      this.checkBox6.Name = "checkBox6";
      this.checkBox6.Size = new System.Drawing.Size(48, 17);
      this.checkBox6.TabIndex = 9;
      this.checkBox6.Text = "Zeta";
      this.checkBox6.UseVisualStyleBackColor = true;
      this.checkBox6.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // checkBox7
      // 
      this.checkBox7.AutoSize = true;
      this.checkBox7.Enabled = false;
      this.checkBox7.Location = new System.Drawing.Point(112, 82);
      this.checkBox7.Name = "checkBox7";
      this.checkBox7.Size = new System.Drawing.Size(42, 17);
      this.checkBox7.TabIndex = 10;
      this.checkBox7.Text = "Eta";
      this.checkBox7.UseVisualStyleBackColor = true;
      this.checkBox7.CheckedChanged += new System.EventHandler(this.CheckBox_CheckedChanged);
      // 
      // loadButton
      // 
      this.loadButton.Location = new System.Drawing.Point(13, 163);
      this.loadButton.Name = "loadButton";
      this.loadButton.Size = new System.Drawing.Size(75, 23);
      this.loadButton.TabIndex = 11;
      this.loadButton.Text = "Load";
      this.loadButton.UseVisualStyleBackColor = true;
      this.loadButton.Click += new System.EventHandler(this.button1_Click);
      // 
      // encryptButton
      // 
      this.encryptButton.Enabled = false;
      this.encryptButton.Location = new System.Drawing.Point(13, 134);
      this.encryptButton.Name = "encryptButton";
      this.encryptButton.Size = new System.Drawing.Size(75, 23);
      this.encryptButton.TabIndex = 13;
      this.encryptButton.Text = "Encrypt pgm";
      this.encryptButton.UseVisualStyleBackColor = true;
      this.encryptButton.Click += new System.EventHandler(this.encryptButton_Click);
      // 
      // decryptButton
      // 
      this.decryptButton.Enabled = false;
      this.decryptButton.Location = new System.Drawing.Point(97, 134);
      this.decryptButton.Name = "decryptButton";
      this.decryptButton.Size = new System.Drawing.Size(75, 23);
      this.decryptButton.TabIndex = 12;
      this.decryptButton.Text = "Decrypt pgm";
      this.decryptButton.UseVisualStyleBackColor = true;
      this.decryptButton.Click += new System.EventHandler(this.decryptButton_Click);
      // 
      // passwordSetButton
      // 
      this.passwordSetButton.Enabled = false;
      this.passwordSetButton.Location = new System.Drawing.Point(13, 105);
      this.passwordSetButton.Name = "passwordSetButton";
      this.passwordSetButton.Size = new System.Drawing.Size(159, 23);
      this.passwordSetButton.TabIndex = 14;
      this.passwordSetButton.Text = "Change Password";
      this.passwordSetButton.UseVisualStyleBackColor = true;
      this.passwordSetButton.Click += new System.EventHandler(this.passwordSetButton_Click);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.AutoSize = true;
      this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.ClientSize = new System.Drawing.Size(183, 193);
      this.Controls.Add(this.passwordSetButton);
      this.Controls.Add(this.encryptButton);
      this.Controls.Add(this.decryptButton);
      this.Controls.Add(this.loadButton);
      this.Controls.Add(this.checkBox7);
      this.Controls.Add(this.checkBox6);
      this.Controls.Add(this.checkBox4);
      this.Controls.Add(this.checkBox5);
      this.Controls.Add(this.checkBox3);
      this.Controls.Add(this.checkBox2);
      this.Controls.Add(this.checkBox1);
      this.Controls.Add(this.ReadOnlyCheckBox);
      this.Controls.Add(this.setButton);
      this.Name = "Form1";
      this.Padding = new System.Windows.Forms.Padding(10);
      this.Text = "Restrict Permissions";
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button setButton;
    private System.Windows.Forms.CheckBox ReadOnlyCheckBox;
    private System.Windows.Forms.CheckBox checkBox1;
    private System.Windows.Forms.CheckBox checkBox2;
    private System.Windows.Forms.CheckBox checkBox3;
    private System.Windows.Forms.CheckBox checkBox4;
    private System.Windows.Forms.CheckBox checkBox5;
    private System.Windows.Forms.CheckBox checkBox6;
    private System.Windows.Forms.CheckBox checkBox7;
    private System.Windows.Forms.Button loadButton;
    private System.Windows.Forms.Button encryptButton;
    private System.Windows.Forms.Button decryptButton;
    private System.Windows.Forms.Button passwordSetButton;


  }
}

