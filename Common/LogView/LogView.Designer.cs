namespace SysCAD.Log
{
  partial class LogView
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

    #region Component Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.SuspendLayout();
      // 
      // LogView
      // 
      this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
      this.columnHeader2 = new System.Windows.Forms.ColumnHeader();

      this.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
      this.Dock = System.Windows.Forms.DockStyle.Fill;
      this.FullRowSelect = true;
      this.GridLines = false;
      this.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
      this.Name = "LogView";
      //this.UseCompatibleStateImageBehavior = false;
      this.View = System.Windows.Forms.View.Details;
      //this.DoubleClick += new System.EventHandler(this.listView1_DoubleClick);
      // 
      // columnHeader1
      // 
      this.columnHeader1.Text = "Source";
      this.columnHeader1.Width = 237;
      // 
      // columnHeader2
      // 
      this.columnHeader2.Text = "Message";
      this.columnHeader2.Width = 500; 
    }

    #endregion

    private System.Windows.Forms.ColumnHeader columnHeader1;
    private System.Windows.Forms.ColumnHeader columnHeader2;
  }
}
