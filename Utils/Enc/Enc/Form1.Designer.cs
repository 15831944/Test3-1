namespace Enc
{
  partial class EncForm
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
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EncForm));
      this.buttonEncrypt = new System.Windows.Forms.Button();
      this.buttonDecrypt = new System.Windows.Forms.Button();
      this.textBoxPassword = new System.Windows.Forms.TextBox();
      this.labelPassword = new System.Windows.Forms.Label();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
      this.tableLayoutPanel1.SuspendLayout();
      this.flowLayoutPanel1.SuspendLayout();
      this.SuspendLayout();
      // 
      // buttonEncrypt
      // 
      this.buttonEncrypt.Location = new System.Drawing.Point(3, 3);
      this.buttonEncrypt.Name = "buttonEncrypt";
      this.buttonEncrypt.Size = new System.Drawing.Size(75, 23);
      this.buttonEncrypt.TabIndex = 0;
      this.buttonEncrypt.Text = "Encrypt";
      this.buttonEncrypt.UseVisualStyleBackColor = true;
      this.buttonEncrypt.Click += new System.EventHandler(this.buttonEncrypt_Click);
      // 
      // buttonDecrypt
      // 
      this.buttonDecrypt.Location = new System.Drawing.Point(84, 3);
      this.buttonDecrypt.Name = "buttonDecrypt";
      this.buttonDecrypt.Size = new System.Drawing.Size(75, 23);
      this.buttonDecrypt.TabIndex = 1;
      this.buttonDecrypt.Text = "Decrypt";
      this.buttonDecrypt.UseVisualStyleBackColor = true;
      this.buttonDecrypt.Click += new System.EventHandler(this.buttonDecrypt_Click);
      // 
      // textBoxPassword
      // 
      this.textBoxPassword.Dock = System.Windows.Forms.DockStyle.Fill;
      this.textBoxPassword.Location = new System.Drawing.Point(3, 51);
      this.textBoxPassword.MaximumSize = new System.Drawing.Size(160, 32767);
      this.textBoxPassword.Name = "textBoxPassword";
      this.textBoxPassword.Size = new System.Drawing.Size(160, 20);
      this.textBoxPassword.TabIndex = 2;
      // 
      // labelPassword
      // 
      this.labelPassword.AutoSize = true;
      this.labelPassword.Location = new System.Drawing.Point(3, 35);
      this.labelPassword.Name = "labelPassword";
      this.labelPassword.Size = new System.Drawing.Size(56, 13);
      this.labelPassword.TabIndex = 3;
      this.labelPassword.Text = "Password:";
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.AutoSize = true;
      this.tableLayoutPanel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.tableLayoutPanel1.ColumnCount = 1;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
      this.tableLayoutPanel1.Controls.Add(this.flowLayoutPanel1, 0, 0);
      this.tableLayoutPanel1.Controls.Add(this.textBoxPassword, 0, 2);
      this.tableLayoutPanel1.Controls.Add(this.labelPassword, 0, 1);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(12, 12);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 3;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
      this.tableLayoutPanel1.Size = new System.Drawing.Size(406, 338);
      this.tableLayoutPanel1.TabIndex = 4;
      // 
      // flowLayoutPanel1
      // 
      this.flowLayoutPanel1.AutoSize = true;
      this.flowLayoutPanel1.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.flowLayoutPanel1.Controls.Add(this.buttonEncrypt);
      this.flowLayoutPanel1.Controls.Add(this.buttonDecrypt);
      this.flowLayoutPanel1.Location = new System.Drawing.Point(3, 3);
      this.flowLayoutPanel1.Name = "flowLayoutPanel1";
      this.flowLayoutPanel1.Size = new System.Drawing.Size(162, 29);
      this.flowLayoutPanel1.TabIndex = 5;
      // 
      // EncForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.AutoSize = true;
      this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
      this.ClientSize = new System.Drawing.Size(430, 362);
      this.Controls.Add(this.tableLayoutPanel1);
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.Name = "EncForm";
      this.Padding = new System.Windows.Forms.Padding(12);
      this.Text = "Enc";
      this.tableLayoutPanel1.ResumeLayout(false);
      this.tableLayoutPanel1.PerformLayout();
      this.flowLayoutPanel1.ResumeLayout(false);
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button buttonEncrypt;
    private System.Windows.Forms.Button buttonDecrypt;
    private System.Windows.Forms.TextBox textBoxPassword;
    private System.Windows.Forms.Label labelPassword;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
  }
}

