namespace SysCAD.Editor
{
  partial class InsertAnnotationDialog
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
      this.insertAnnotationTabControl = new System.Windows.Forms.TabControl();
      this.simpleTabPage = new System.Windows.Forms.TabPage();
      this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
      this.tagLabel = new System.Windows.Forms.Label();
      this.tagListBox = new System.Windows.Forms.ListBox();
      this.annotationListBox = new System.Windows.Forms.ListBox();
      this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
      this.cancelButton = new System.Windows.Forms.Button();
      this.okButton = new System.Windows.Forms.Button();
      this.annotationLabel = new System.Windows.Forms.Label();
      this.expertTabPage = new System.Windows.Forms.TabPage();
      this.xamlTextBox = new System.Windows.Forms.TextBox();
      this.insertAnnotationTabControl.SuspendLayout();
      this.simpleTabPage.SuspendLayout();
      this.tableLayoutPanel1.SuspendLayout();
      this.tableLayoutPanel2.SuspendLayout();
      this.expertTabPage.SuspendLayout();
      this.SuspendLayout();
      // 
      // insertAnnotationTabControl
      // 
      this.insertAnnotationTabControl.Controls.Add(this.simpleTabPage);
      this.insertAnnotationTabControl.Controls.Add(this.expertTabPage);
      this.insertAnnotationTabControl.Dock = System.Windows.Forms.DockStyle.Fill;
      this.insertAnnotationTabControl.Location = new System.Drawing.Point(0, 0);
      this.insertAnnotationTabControl.Name = "insertAnnotationTabControl";
      this.insertAnnotationTabControl.SelectedIndex = 0;
      this.insertAnnotationTabControl.Size = new System.Drawing.Size(373, 470);
      this.insertAnnotationTabControl.TabIndex = 0;
      // 
      // simpleTabPage
      // 
      this.simpleTabPage.Controls.Add(this.tableLayoutPanel1);
      this.simpleTabPage.Location = new System.Drawing.Point(4, 22);
      this.simpleTabPage.Name = "simpleTabPage";
      this.simpleTabPage.Size = new System.Drawing.Size(365, 444);
      this.simpleTabPage.TabIndex = 0;
      this.simpleTabPage.Text = "Simple";
      // 
      // tableLayoutPanel1
      // 
      this.tableLayoutPanel1.ColumnCount = 2;
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 19.72603F));
      this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 80.27397F));
      this.tableLayoutPanel1.Controls.Add(this.tagLabel, 0, 0);
      this.tableLayoutPanel1.Controls.Add(this.tagListBox, 1, 0);
      this.tableLayoutPanel1.Controls.Add(this.annotationListBox, 1, 1);
      this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel2, 1, 2);
      this.tableLayoutPanel1.Controls.Add(this.annotationLabel, 0, 1);
      this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
      this.tableLayoutPanel1.Name = "tableLayoutPanel1";
      this.tableLayoutPanel1.RowCount = 3;
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 34F));
      this.tableLayoutPanel1.Size = new System.Drawing.Size(365, 444);
      this.tableLayoutPanel1.TabIndex = 1;
      // 
      // tagLabel
      // 
      this.tagLabel.AutoSize = true;
      this.tagLabel.Location = new System.Drawing.Point(3, 0);
      this.tagLabel.Name = "tagLabel";
      this.tagLabel.Size = new System.Drawing.Size(65, 13);
      this.tagLabel.TabIndex = 0;
      this.tagLabel.Text = "Linked Item:";
      // 
      // tagListBox
      // 
      this.tagListBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tagListBox.FormattingEnabled = true;
      this.tagListBox.Location = new System.Drawing.Point(75, 3);
      this.tagListBox.Name = "tagListBox";
      this.tagListBox.Size = new System.Drawing.Size(287, 199);
      this.tagListBox.Sorted = true;
      this.tagListBox.TabIndex = 1;
      this.tagListBox.SelectedIndexChanged += new System.EventHandler(this.tagListBox_SelectedIndexChanged);
      // 
      // annotationListBox
      // 
      this.annotationListBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.annotationListBox.Enabled = false;
      this.annotationListBox.FormattingEnabled = true;
      this.annotationListBox.Location = new System.Drawing.Point(75, 208);
      this.annotationListBox.Name = "annotationListBox";
      this.annotationListBox.Size = new System.Drawing.Size(287, 199);
      this.annotationListBox.TabIndex = 3;
      this.annotationListBox.SelectedIndexChanged += new System.EventHandler(this.annotationListBox_SelectedIndexChanged);
      // 
      // tableLayoutPanel2
      // 
      this.tableLayoutPanel2.ColumnCount = 2;
      this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.Controls.Add(this.cancelButton, 1, 0);
      this.tableLayoutPanel2.Controls.Add(this.okButton, 0, 0);
      this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tableLayoutPanel2.Location = new System.Drawing.Point(75, 413);
      this.tableLayoutPanel2.Name = "tableLayoutPanel2";
      this.tableLayoutPanel2.RowCount = 1;
      this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
      this.tableLayoutPanel2.Size = new System.Drawing.Size(287, 28);
      this.tableLayoutPanel2.TabIndex = 4;
      // 
      // cancelButton
      // 
      this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
      this.cancelButton.Location = new System.Drawing.Point(146, 3);
      this.cancelButton.Name = "cancelButton";
      this.cancelButton.Size = new System.Drawing.Size(75, 22);
      this.cancelButton.TabIndex = 0;
      this.cancelButton.Text = "Cancel";
      this.cancelButton.UseVisualStyleBackColor = true;
      // 
      // okButton
      // 
      this.okButton.Cursor = System.Windows.Forms.Cursors.Default;
      this.okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
      this.okButton.Enabled = false;
      this.okButton.Location = new System.Drawing.Point(3, 3);
      this.okButton.Name = "okButton";
      this.okButton.Size = new System.Drawing.Size(75, 22);
      this.okButton.TabIndex = 1;
      this.okButton.Text = "OK";
      this.okButton.UseVisualStyleBackColor = true;
      // 
      // annotationLabel
      // 
      this.annotationLabel.AutoSize = true;
      this.annotationLabel.Location = new System.Drawing.Point(3, 205);
      this.annotationLabel.Name = "annotationLabel";
      this.annotationLabel.Size = new System.Drawing.Size(61, 13);
      this.annotationLabel.TabIndex = 2;
      this.annotationLabel.Text = "Annotation:";
      // 
      // expertTabPage
      // 
      this.expertTabPage.Controls.Add(this.xamlTextBox);
      this.expertTabPage.Location = new System.Drawing.Point(4, 22);
      this.expertTabPage.Name = "expertTabPage";
      this.expertTabPage.Size = new System.Drawing.Size(365, 444);
      this.expertTabPage.TabIndex = 1;
      this.expertTabPage.Text = "Expert";
      // 
      // xamlTextBox
      // 
      this.xamlTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.xamlTextBox.Location = new System.Drawing.Point(0, 0);
      this.xamlTextBox.Multiline = true;
      this.xamlTextBox.Name = "xamlTextBox";
      this.xamlTextBox.Size = new System.Drawing.Size(365, 444);
      this.xamlTextBox.TabIndex = 0;
      // 
      // InsertAnnotationDialog
      // 
      this.AcceptButton = this.okButton;
      this.CancelButton = this.cancelButton;
      this.ClientSize = new System.Drawing.Size(373, 470);
      this.Controls.Add(this.insertAnnotationTabControl);
      this.Name = "InsertAnnotationDialog";
      this.Text = "InsertAnnotationDialog";
      this.insertAnnotationTabControl.ResumeLayout(false);
      this.simpleTabPage.ResumeLayout(false);
      this.tableLayoutPanel1.ResumeLayout(false);
      this.tableLayoutPanel1.PerformLayout();
      this.tableLayoutPanel2.ResumeLayout(false);
      this.expertTabPage.ResumeLayout(false);
      this.expertTabPage.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.TabControl insertAnnotationTabControl;
    private System.Windows.Forms.TabPage expertTabPage;
    private System.Windows.Forms.TabPage simpleTabPage;
    private System.Windows.Forms.Label tagLabel;
    private System.Windows.Forms.ListBox tagListBox;
    private System.Windows.Forms.Label annotationLabel;
    private System.Windows.Forms.ListBox annotationListBox;
    private System.Windows.Forms.TextBox xamlTextBox;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
    private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
    private System.Windows.Forms.Button cancelButton;
    private System.Windows.Forms.Button okButton;
  }
}