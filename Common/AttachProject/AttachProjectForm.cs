
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using SysCAD.Protocol;

//using System.Security.Permissions;

namespace SysCAD
{

  public partial class AttachProjectForm : Form
  {

    private ClientProtocol clientProtocol = new ClientProtocol();

    private Config config = new Config();

    public AttachProjectForm()
    {
      InitializeComponent();

      if (config.TestUrl(new System.Uri(repositoryURLTextBox.Text + "Global")))
      {
        RefreshProjectList();
      }
    }

    public AttachProjectForm(AttachProjectForm previousAttachProjectForm)
    {
      clientProtocol = previousAttachProjectForm.clientProtocol;
      config = previousAttachProjectForm.config;
    }

    private void RefreshProjectList()
    {
      addButton.Enabled = true;
      config.Syncxxx();
      config.GetProjectList();
      projectListBox.Items.Clear();

      foreach (String projectString in config.ProjectList)
      {
        projectListBox.Items.Add(projectString);
      }

      projectListBox.Enabled = true;
      if (projectListBox.Items.Count > 0)
      {
        projectListBox.SelectedIndex = 0;
      }

      projectListBox.Enabled = true;
    }

    private void listProjectsButton_Click(object sender, EventArgs e)
    {

      if (config.TestUrl(new System.Uri(repositoryURLTextBox.Text + "Global")))
      {
        RefreshProjectList();
      }
      else
      {
        projectListBox.Items.Clear();
        projectListBox.Enabled = false;
      }
      errorProvider1.SetError(projectListBox, config.connectionError);
    }

    private void projectListBox_DoubleClick(object sender, EventArgs e)
    {
      attachButton.PerformClick();
    }

    private void projectListBox_KeyPress(object sender, KeyPressEventArgs e)
    {

      if (e.KeyChar == '\r')
      {
        attachButton.PerformClick();
      }
      e.Handled = true;
    }

    private void projectListBox_SelectedValueChanged(object sender, EventArgs e)
    {

      if (projectListBox.SelectedItem != null)
      {

        if (clientProtocol.TestUrl(new System.Uri(repositoryURLTextBox.Text + "Client/" + projectListBox.SelectedItem.ToString())))
        {
          attachButton.Enabled = true;
        }

        errorProvider1.SetError(projectListBox, clientProtocol.connectionError);
      }
    }

    private void repositoryURLTextBox_KeyPress(object sender, KeyPressEventArgs e)
    {

      if (e.KeyChar == '\r')
      {
        listProjectsButton.PerformClick();
        e.Handled = true;
      } 
    }

    public ClientProtocol ClientProtocol
    {
      get { return clientProtocol; }
    }

    public Config Config
    {
      get { return config; }
    }

    private void addButton_Click(object sender, EventArgs e)
    {
      AddProjectForm addProjectForm = new AddProjectForm();
      if (addProjectForm.ShowDialog(this) == DialogResult.OK)
      {
        config.Syncxxx();
        config.AddProject(addProjectForm.projectName, addProjectForm.projectPath);
        RefreshProjectList();
      }
    }
  }
}