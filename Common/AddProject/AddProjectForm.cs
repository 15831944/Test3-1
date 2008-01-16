
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

      while (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
      {
        if (Directory.Exists(folderBrowserDialog1.SelectedPath))
        {
          // Just check for Project.spj at the moment, in future do full check.
          if (Directory.GetFiles(folderBrowserDialog1.SelectedPath, "Project.spj").Length != 0)
          {
            pathTextBox.Text = folderBrowserDialog1.SelectedPath;
            if (String.IsNullOrEmpty(nameTextBox.Text))
              nameTextBox.Text = Path.GetFileNameWithoutExtension(folderBrowserDialog1.SelectedPath);
            nameTextBox.Focus();
            return;
          }
        }

        MessageBox.Show("The selected folder doesn't contain a valid SysCAD project.", "Not a project folder.");
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
