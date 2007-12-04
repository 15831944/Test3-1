
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace SysCAD
{

  public partial class AddProjectForm : Form
  {

    public AddProjectForm()
    {
      InitializeComponent();
    }

    private void browseButton_Click(object sender, EventArgs e)
    {
      folderBrowserDialog1.SelectedPath = pathTextBox.Text;
      folderBrowserDialog1.ShowDialog();

      if (Directory.Exists(folderBrowserDialog1.SelectedPath))
      {
        pathTextBox.Text = folderBrowserDialog1.SelectedPath;
        if (String.IsNullOrEmpty(nameTextBox.Text))
          nameTextBox.Text = Path.GetFileNameWithoutExtension(folderBrowserDialog1.SelectedPath);
        nameTextBox.Focus();
      }
    }

    public string projectName
    {
      get { return nameTextBox.Text; }
      set { nameTextBox.Text = value; }
    }

    public string projectPath
    {
      get { return pathTextBox.Text; }
      set { pathTextBox.Text = value; }
    }

    private void TextBox_TextChanged(object sender, EventArgs e)
    {
      if ((nameTextBox.Text.Length > 0) && (pathTextBox.Text.Length > 0) && (Directory.Exists(pathTextBox.Text)))
      {
        if ((Directory.GetFiles(pathTextBox.Text, "Graphic.10").Length == 1) && (Directory.GetFiles(pathTextBox.Text, "Model.10").Length == 1))
        //if (project.AddProject(project))
        {
          addButton.Enabled = true;
          return;
        }
      }

      addButton.Enabled = false;
    }
  }
}
